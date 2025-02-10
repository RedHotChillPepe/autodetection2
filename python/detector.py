import cv2
import numpy as np
from ultralytics import YOLO
from config import DETECTION_ZONES, OBJECT_CLASSES
# from acllite.acllite_model import AclLiteModel

# model1 = YOLO("yolo11n.pt")
model1 = YOLO("models/cars11_v1.1.pt")

# model_path = "yolo_best_npu_model.om"
# model1 = AclLiteModel(model_path)
class ObjectDetector:
    def __init__(self, model_path, device='npu'):
        # Инициализация модели YOLO
        self.device = device
        self.model = model1

    def detect_objects(self, frame, confidence_threshold=0.5):
        results = self.model.predict(frame, device=self.device)
        detections = []

        # Обработка результатов
        for result in results:
            boxes = result.boxes
            for i, box in enumerate(boxes):
                x1, y1, x2, y2 = box.xyxy[0]  # Координаты объекта
                cls_index = int(box.cls[0])  # Класс объекта в формате числа
                confidence = float(box.conf[0])  # Уверенность детекции

                if confidence > confidence_threshold:

                    # Проверка, существует ли класс в OBJECT_CLASSES
                    if cls_index < len(OBJECT_CLASSES):
                        obj_class = list(OBJECT_CLASSES.keys())[cls_index]
                        print(
                            f"Frame: Object {i} detected with class '{obj_class}', Index: {cls_index}, Confidence: {confidence:.2f}")
                    else:
                        # obj_class = 'unknown'
                        # print(f"Warning: Object with index {cls_index} has an unknown class")
                        continue

                    detections.append({
                        'bbox': [x1, y1, x2, y2],
                        'class': obj_class,
                        'confidence': confidence
                    })
                else:
                    continue

        return detections

    def postprocess(self, results):
        # Обработка результатов инференса
        detections = []
        for result in results:
            for box in result.boxes:
                detections.append({
                    'bbox': box.xyxy[0].tolist(),  # Координаты ограничивающего прямоугольника
                    'confidence': float(box.conf[0]),  # Уверенность модели
                    'class': int(box.cls[0])  # Класс объекта
                })
        return detections