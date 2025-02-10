# Настройки классов объектов и зон детекции
import numpy as np

# Классы объектов
OBJECT_CLASSES = {
    'car': 0,
    'lightCommercialVehicle': 1,
    'truck': 2,
    'bus': 3,
    'trailer': 4,
    'bicycle': 5,
    'motorcycle': 6,
    'tractor': 7,
    'tram': 8,
    'pedestrian': 9,
}

# Зоны детекции
DETECTION_ZONES = [
    {
        'polygon': np.array([
            [166, 428],
            [163, 408],
            [1061, 398],
            [1085, 412]
        ]),
        'name': 'zone_1',
        'counts': {
            'car': 0,
            'lightCommercialVehicle': 0,
            'truck': 0,
            'bus': 0,
            'trailer': 0,
            'bicycle': 0,
            'motorcycle': 0,
            'tractor': 0,
            'tram': 0,
            'pedestrian': 0,
        }
    }
]

# Цвета для каждого класса объектов
CLASS_COLORS = {
    'car': (0, 255, 255),
    'lightCommercialVehicle': (255, 0, 0),
    'truck': (0, 255, 0),
    'bus': (0, 0, 255),
    'trailer': (255, 255, 0),
    'bicycle': (255, 0, 255),
    'motorcycle': (0, 255, 255),
    'tractor': (255, 165, 0),
    'tram': (128, 0, 128),
    'pedestrian': (100, 100, 128),
}