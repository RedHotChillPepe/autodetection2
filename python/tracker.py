import math
import numpy as np
from collections import deque

class ObjectTracker:
    def __init__(self, max_disappeared=10, max_distance=50):
        self.center_points = {}
        self.disappeared = {}
        self.tracks = {}
        self.id_count = 0
        self.max_disappeared = max_disappeared
        self.max_distance = max_distance
        self.min_hits = 3
        self.track_history = {}

    def update_tracks(self, detections, frame):
        objects_rect = []
        for det in detections:
            x1, y1, x2, y2 = det['bbox']
            width = x2 - x1
            height = y2 - y1
            objects_rect.append([x1, y1, width, height])

        tracked_objects = self.update(objects_rect)
        
        track_results = []
        for obj_bb_id in tracked_objects:
            x, y, w, h, obj_id = obj_bb_id
            if obj_id in self.tracks and self.tracks[obj_id]['hits'] >= self.min_hits:
                track_results.append({
                    'id': obj_id,
                    'bbox': [x, y, x + w, y + h],
                    'confidence': self.tracks[obj_id].get('confidence', 1.0)
                })

        return track_results

    def update(self, objects_rect):
        objects_bbs_ids = []
        
        # Обработка текущих детекций
        current_centers = {}
        for rect in objects_rect:
            x, y, w, h = rect
            cx = (x + x + w) // 2
            cy = (y + y + h) // 2
            current_centers[(cx, cy)] = (x, y, w, h)

        # Сопоставление существующих треков с новыми детекциями
        if len(self.center_points) == 0:
            for center, rect in current_centers.items():
                x, y, w, h = rect
                self.register_new_track(center, rect)
                objects_bbs_ids.append([x, y, w, h, self.id_count - 1])
        else:
            track_ids = list(self.center_points.keys())
            current_centers_list = list(current_centers.keys())

            # Вычисление матрицы расстояний
            distances = np.zeros((len(track_ids), len(current_centers_list)))
            for i, track_id in enumerate(track_ids):
                for j, center in enumerate(current_centers_list):
                    distances[i, j] = self.calculate_distance(
                        self.center_points[track_id], center)

            # Сопоставление треков с детекциями
            matched_tracks = set()
            matched_detections = set()

            while True:
                if distances.size == 0 or distances.min() > self.max_distance:
                    break

                i, j = np.unravel_index(distances.argmin(), distances.shape)
                track_id = track_ids[i]
                center = current_centers_list[j]

                if track_id not in matched_tracks and j not in matched_detections:
                    x, y, w, h = current_centers[center]
                    self.update_track(track_id, center, (x, y, w, h))
                    objects_bbs_ids.append([x, y, w, h, track_id])
                    matched_tracks.add(track_id)
                    matched_detections.add(j)

                distances[i, j] = float('inf')

            # Обработка непривязанных треков
            for track_id in track_ids:
                if track_id not in matched_tracks:
                    self.handle_disappeared_track(track_id)

            # Регистрация новых треков
            for j, center in enumerate(current_centers_list):
                if j not in matched_detections:
                    x, y, w, h = current_centers[center]
                    self.register_new_track(center, (x, y, w, h))
                    objects_bbs_ids.append([x, y, w, h, self.id_count - 1])

        # Очистка старых треков
        self.cleanup_tracks()
        return objects_bbs_ids

    def calculate_distance(self, point1, point2):
        return math.hypot(point1[0] - point2[0], point1[1] - point2[1])

    def register_new_track(self, center, rect):
        self.center_points[self.id_count] = center
        self.disappeared[self.id_count] = 0
        self.tracks[self.id_count] = {
            'hits': 1,
            'no_losses': 0,
            'rect': rect
        }
        if self.id_count not in self.track_history:
            self.track_history[self.id_count] = deque(maxlen=30)
        self.track_history[self.id_count].append(center)
        self.id_count += 1

    def update_track(self, track_id, center, rect):
        self.center_points[track_id] = center
        self.disappeared[track_id] = 0
        self.tracks[track_id]['hits'] += 1
        self.tracks[track_id]['no_losses'] += 1
        self.tracks[track_id]['rect'] = rect
        self.track_history[track_id].append(center)

    def handle_disappeared_track(self, track_id):
        self.disappeared[track_id] += 1
        self.tracks[track_id]['no_losses'] = 0

    def cleanup_tracks(self):
        # Удаление старых треков
        for track_id in list(self.disappeared.keys()):
            if self.disappeared[track_id] > self.max_disappeared:
                self.delete_track(track_id)

    def delete_track(self, track_id):
        del self.center_points[track_id]
        del self.disappeared[track_id]
        del self.tracks[track_id]
        del self.track_history[track_id]

    def get_track_history(self, track_id):
        return list(self.track_history.get(track_id, []))
