import numpy as np
import time
from collections import deque

class SpeedEstimator:
    def __init__(self, homography_matrix, fps):
        self.homography_matrix = homography_matrix  # Матрица гомографии
        self.fps = fps  # Частота кадров
        # self.last_positions = {}  # Последние позиции объектов
        # self.last_times = {}  # Время последнего обновления объекта
        self.buffers = {}  # Буферы для хранения позиций и времени (по объектам)

    def apply_homography(self, point):
        px, py = point
        point_homogeneous = np.array([px, py, 1])
        transformed_point = np.dot(self.homography_matrix, point_homogeneous)
        transformed_point /= transformed_point[2]  # Нормализация
        return transformed_point[:2]

    def estimate_speed(self, object_id, bbox_center, bbox):
        current_time = time.time()
        transformed_center = self.apply_homography(bbox_center)

        # Получаем высоту объекта на экране
        object_height = bbox[3] - bbox[1]

        # Добавляем корректировку на высоту объекта
        # perspective_factor = 1 / (object_height / 500)  # Коэффициент корректировки на высоту
        # print(f"Object height: {object_height}, Perspective factor: {perspective_factor}")

        # Инициализируем буфер для объекта, если его нет
        if object_id not in self.buffers:
            self.buffers[object_id] = deque(maxlen=self.fps)  # Храним данные за 1 секунду
            self.buffers[object_id].append((transformed_center, current_time))
            return 0.0  # Возвращаем 0, так как недостаточно данных для расчета скорости

        # Добавляем текущую позицию и время в буфер
        self.buffers[object_id].append((transformed_center, current_time))

        # Если буфер ещё не заполнен (меньше 30 кадров), скорость не рассчитывается
        if len(self.buffers[object_id]) >= 2:  # Достаточно двух точек для расчета скорости
            # Получаем первую и последнюю запись из буфера
            first_position, first_time = self.buffers[object_id][0]
            last_position, last_time = self.buffers[object_id][-1]

            # Рассчитываем расстояние между первой и последней точкой
            distance_m = np.linalg.norm(np.array(last_position) - np.array(first_position))

            # Рассчитываем временной интервал
            delta_time = last_time - first_time
            if delta_time == 0:  # Защита от деления на 0
                return 0.0

            # Рассчитываем скорость
            speed_m_per_s = distance_m / delta_time
            speed_kmh = speed_m_per_s * 3.6

            # Если буфер ещё не заполнен полностью, возвращаем начальную скорость
            if len(self.buffers[object_id]) < self.fps:
                print(f"Initial speed for object {object_id}: {speed_kmh} km/h")
                return speed_kmh

            # Если буфер заполнен, продолжаем обычный расчет
            return speed_kmh

            # Если буфер пуст, скорость недоступна
        return 0.0

    def apply_homography(self, point):
        """
        Преобразует координаты точки с учетом матрицы гомографии.
        :param point: Точка (x, y).
        :return: Преобразованная точка (x', y').
        """
        px, py = point
        point_homogeneous = np.array([px, py, 1])
        transformed_point = np.dot(self.homography_matrix, point_homogeneous)
        transformed_point /= transformed_point[2]
        return transformed_point[:2]

    def initialize_tracking(self, object_id, position, time):
        self.last_positions[object_id] = position
        self.last_times[object_id] = time
        self.speed_history[object_id] = deque(maxlen=self.history_window)
        print(f"Initialized tracking for object {object_id} at position {position} and time {time}")
