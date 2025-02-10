from fastapi import FastAPI, UploadFile, File
from detector import ObjectDetector
from tracker import ObjectTracker
from speed_estimator import SpeedEstimator
from config import DETECTION_ZONES, OBJECT_CLASSES, CLASS_COLORS
import cv2
import numpy as np
import threading
from fastapi.responses import StreamingResponse
import io
import time
from ultralytics import YOLO
# from acllite.acllite_model import AclLiteModel

app = FastAPI()

# model = YOLO("yolo11n.pt")
model = YOLO("models/cars11_v1.1.pt")

# model_path = "yolo_best_npu_model.om"
# model = AclLiteModel(model_path)

# Инициализация модулей
object_detector = ObjectDetector(model_path=model, device='cpu')
object_tracker = ObjectTracker()

src_points = np.array([
    [161, 409],
    [1056, 405],
    [1670, 396],
    [944, 178],
    [317, 1],
    [232, 1],
    [110, 1],
    [133, 172]
], dtype=np.float32)

dst_points = np.array([
    [0, 0],
    [10, 0],
    [20, 0],
    [20, 40],
    [20, 200],
    [10, 200],
    [0, 200],
    [0, 40]
], dtype=np.float32)

homography_matrix, _ = cv2.findHomography(src_points, dst_points, cv2.RANSAC)
print(homography_matrix)

# Масштаб: 0.04 метра на пиксель, частота кадров: 30 кадров в секунду
fps = 30
speed_estimator = SpeedEstimator(homography_matrix, fps)

video_capture = None
processed_object_ids = set()  # Множество для хранения ID объектов, уже учтенных в зоне

def RGB(event, x, y, flags, param):
    if event == cv2.EVENT_MOUSEMOVE:
        colorBGR = [x, y]
        print(colorBGR)

@app.post("/process_frame/")
async def process_frame(file: UploadFile = File(...)):
    # Загрузка и декодирование изображения
    contents = await file.read()
    np_arr = np.frombuffer(contents, np.uint8)
    frame = cv2.imdecode(np_arr, cv2.IMREAD_COLOR)

    # Детектирование объектов
    detections = object_detector.detect_objects(frame)

    # Трекинг объектов
    tracks = object_tracker.update_tracks(detections, frame)

    # Расчет скорости для каждого объекта
    results = []
    for track in tracks:
        object_id = track['id']
        bbox = track['bbox']
        bbox_center = [(bbox[0] + bbox[2]) // 2, (bbox[1] + bbox[3]) // 2]
        speed = speed_estimator.estimate_speed(object_id, bbox_center)
        results.append({
            'id': object_id,
            'bbox': bbox,
            'speed': speed
        })

    return {'results': results}

@app.post("/start_video_processing/")
async def start_video_processing(video_url: str):
    global video_capture
    video_capture = cv2.VideoCapture(video_url)
    threading.Thread(target=process_video).start()
    return {"status": "Video processing started"}

def RGB(event, x, y, flags, param):
    if event == cv2.EVENT_MOUSEMOVE:
        colorBGR = [x, y]
        print(colorBGR)

def process_video():
    global video_capture
    frame_count = 1
    while video_capture.isOpened():
        ret, frame = video_capture.read()
        if not ret:
            break

        # Сохраняем текущее время для расчета скорости
        current_time = time.time()

        # Детектирование объектов
        detections = object_detector.detect_objects(frame)

        # Трекинг объектов
        tracks = object_tracker.update_tracks(detections, frame)

        # Добавляем классы к трекам
        for detection in detections:
            x1, y1, x2, y2 = detection['bbox']
            obj_class = detection.get('class')
            confidence = detection.get('confidence')

            # Привязываем классы к объектам в треках
            for track in tracks:
                track_bbox = track['bbox']
                if (
                        abs(track_bbox[0] - x1) < 10 and
                        abs(track_bbox[1] - y1) < 10 and
                        abs(track_bbox[2] - x2) < 10 and
                        abs(track_bbox[3] - y2) < 10
                ):
                    track['class'] = obj_class
                    track['confidence'] = confidence

        # Нанесение детекций, треков и расчет скорости
        for track in tracks:
            object_id = track['id']
            x1, y1, x2, y2 = track['bbox']
            bbox = track['bbox']
            obj_class = track.get('class')  # Получаем класс объекта
            confidence = detection.get('confidence')
            print(f"Класс для объекта {object_id}: {obj_class}")

            # Приведение координат к целочисленному типу
            x1, y1, x2, y2 = int(x1), int(y1), int(x2), int(y2)

            # Расчет скорости объекта
            bbox_center = [(x1 + x2) // 2, (y1 + y2) // 2]
            bbox_bottom_center = [(x1 + x2) // 2, y2]  # Точка учета внизу прямоугольника
            speed = speed_estimator.estimate_speed(object_id, bbox_center, bbox)

            # Определение цвета для класса объекта
            color = CLASS_COLORS.get(obj_class)  # Используем цвет для класса, если он определен
            print(f"Цвет для объекта {object_id}: {color}")

            # Рисуем прямоугольник вокруг объекта и отображаем скорость в км/ч
            cv2.rectangle(frame, (x1, y1), (x2, y2), color, 2)
            cv2.putText(frame, f'ID: {object_id}, Class: {obj_class}, Speed: {speed:.2f} km/h',
                        (x1, y1 - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, color, 2)

            # Отрисовка точки внизу центра прямоугольника объекта
            cv2.circle(frame, (bbox_bottom_center[0], bbox_bottom_center[1]), 5, (255, 0, 0), -1)

            # Проверяем пересечение с рабочими зонами
            for zone in DETECTION_ZONES:
                polygon = zone['polygon']
                # Проверка пересечения точки внизу bbox с полигоном зоны
                inside_zone = cv2.pointPolygonTest(polygon, (bbox_bottom_center[0], bbox_bottom_center[1]), False)
                if inside_zone >= 0 and object_id not in processed_object_ids:
                    print(f"Object {object_id} with class '{obj_class}' is in zone '{zone['name']}'")

                    # Инкрементируем счетчик для класса объекта в зоне
                    if obj_class in zone['counts']:
                        zone['counts'][obj_class] += 1
                        processed_object_ids.add(object_id)  # Добавляем объект в список уже учтенных
                        print(f"Incremented count for '{obj_class}' in zone '{zone['name']}'")
                    else:
                        print(f"Warning: Class '{obj_class}' not found in zone counts")

        # Отрисовка рабочих зон и счетчиков
        for zone in DETECTION_ZONES:
            polygon = zone['polygon']
            # Рисуем полигон зоны
            cv2.polylines(frame, [polygon], isClosed=True, color=(255, 0, 0), thickness=2)
            # Рисуем счетчики
            count_text = f"{zone['name']} Counts: " + ", ".join(
                [f"{cls}: {count}" for cls, count in zone['counts'].items()])
            cv2.putText(frame, count_text,
                        (polygon[0][0], polygon[0][1] - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 0, 0), 2)

        print("Номер кадра: ", str(frame_count))
        frame_count += 1
        # Показываем обработанный кадр
        frame_resized = cv2.resize(frame, (1920, 1080))
        cv2.imshow('Detections', frame_resized)
        cv2.setMouseCallback('Detections', RGB)
        if cv2.waitKey(1) & 0xFF == 32:
            while True:
                key = cv2.waitKey(1) & 0xFF
                if key == 32:
                    print("Пауза: нажата клавиша Space.")
                    break

        if cv2.waitKey(1) & 0xFF == 27:
            print("Завершение: нажата клавиша Esc.")
            break

    video_capture.release()
    cv2.destroyAllWindows()


@app.get("/video_feed/")
async def video_feed():
    return StreamingResponse(generate_frames(), media_type="multipart/x-mixed-replace; boundary=frame")