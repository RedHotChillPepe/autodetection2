#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import argparse
import torch

# ---------------------------------------------------------------------
# 1. Подключим наши кастомные операторы (DecBBox, Filter, NMS, Sort).
#    Допустим, они лежат в папке rpn_op, как в вашем патче.
# ---------------------------------------------------------------------
sys.path.append("./rpn_op")

import dec_bbox   # внутри dec_bbox.py уже есть torch.utils.cpp_extension.load_inline(...)
import filter     # внутри filter.py ...
import nms        # внутри nms.py ...
import sort       # внутри sort.py ...

# ---------------------------------------------------------------------
# 2. Импорт библиотеки ultralytics
#    ВАЖНО: если у вас отдельная virtualenv с ultralytics,
#    убедитесь, что у вас активирована среда/версия python,
#    где ultralytics уже установлен (pip install ultralytics)
#    или вы используете локальный репозиторий исходников.
# ---------------------------------------------------------------------
from ultralytics import YOLO

# Пример, как можно «подцепить» класс Detect для monkey-patch, если нужно
# (В зависимости от версии ultralytics путь может отличаться):
try:
    from ultralytics.nn.head.head import Detect
except ImportError:
    raise RuntimeError("Не удалось импортировать класс Detect из ultralytics.nn.head.head. "
                       "Проверьте структуру библиотеки Ultralytics.")


# Сохраняем оригинальный метод _inference
_original_inference = Detect._inference


def _patched_inference(self, x):
    """
    Monkey-patch замена для Detect._inference(x).
    После вычисления dbox+cls мы вставим наши кастомные слои DecBBox, Filter, Sort, NMS.

    В старом патче структура была:
      box, cls = x_cat.split((64, 80), 1)
      tmp = torch.cat((box, cls), 1).view(1, 1, 144, -1)
      -> DecBBox -> Filter -> Sort -> NMS

    Однако в новой версии Ultralytics _inference уже возвращает готовый тензор:
      y = cat((dbox, cls), 1) размером [B, 4 + nc, anchors]
      (после DFL + sigmoid для cls).
    """
    # Вызываем оригинальный _inference, чтобы получить "сырой" вывод сети (без дополнительного postprocess)
    y = _original_inference(self, x)  # torch.Size([B, 4+nc, anchors]) (обычно)

    if not isinstance(y, torch.Tensor):
        # Если вернулось что-то иное, выведем предупреждение
        print("WARNING: _original_inference() вернул не тензор:", type(y))
        return y

    # ---- Пример: если у вас 80 классов и 4 координаты (уже после DFL), тогда:
    #      y.shape = [1, 84, N]   (4 на bbox, 80 на cls)
    # Посмотрите реальную форму:
    b, c, a = y.shape  # batch, channels, anchors
    # Для отладки можно сделать print(f"DEBUG: y.shape={y.shape}")

    # Разделим обратно на box (4) и cls (c-4).
    # Если у вас число классов не 80, а другое — учтите это.
    box, cls = y.split([4, c - 4], dim=1)

    # Превратим в [B, 1, c, a], как в вашем старом патче (1 вместо "nl" дет.голов)
    tmp = torch.cat((box, cls), dim=1).view(b, 1, c, -1)  # [B,1,c,anchors]

    # ---- Теперь вызываем наши кастомные слои:
    # Заметьте, в вашем патче часто были жёстко зашиты gridH=20, gridW=20, num_anchors=400.
    # Если ваша модель реально даёт 8400 ячеек, нужно ставить gridH=20, gridW=20 * 3 паф?
    # Или  [80,80], [40,40], [20,20] и т.д. - решайте сами.
    # Ниже - пример "жёстких" значений:
    decbbox_layer = dec_bbox.DecBBox(
        y=[0.0],
        num_anchors=a,  # количество "якорей" = a,
        num_coords=4,
        num_classes=c - 4,  # здесь это кол-во классов
        gridH=20,
        gridW=20,
        imgW=640,
        imgH=640,
        useClassId=1,
        calc_mode=8,
        clip_bbox=1,
        share_loc=1,
        multi_label=0
    )
    tmp = decbbox_layer(tmp)

    filter_layer = filter.Filter(topK=300, conf_thres=0.9)
    tmp = filter_layer(tmp)

    sort_layer = sort.Sort(topK=300, multi_sort=0, class_num=1)
    tmp = sort_layer(tmp)

    nms_layer = nms.NMS(topK=300, nms_thresh=0.5)
    tmp = nms_layer(tmp)

    # Возвращаем результат (тензор с учётом кастомных слоёв)
    return tmp


# Заменяем метод _inference на наш
Detect._inference = _patched_inference
def export_model(weights_path: str, onnx_path: str = None, opset: int = 13):
    """
    Экспортирует модель YOLO (v5/v8/и т.д.) в ONNX с учётом кастомных слоёв.
    :param weights_path: путь к .pt-файлу (кастомно обученному).
    :param onnx_path:   выходной .onnx (если None, то формируется автоматически).
    :param opset:       версия opset ONNX (по умолчанию 13).
    """

    # weights_path = "models/best.pt"
    print(f"Loading YOLO model from: {weights_path}")
    model = YOLO(weights_path)

    # Переопределяем выходной путь, если не задан
    if onnx_path is None:
        onnx_path = weights_path.replace('.pt', '.onnx')

    print(f"Exporting to ONNX: {onnx_path} (opset={opset})")
    model.export(
        format="onnx",
        opset=opset,
        half=False,         # при желании можно оставить half=True
        simplify=False,     # при необходимости - используйте onnx-simplifier
        dynamic=False,      # True, если хотите динамические размеры
        export_dir=".",     # куда сохранить onnx (.)
        name=onnx_path      # конечное имя файла
    )
    print("Export finished successfully!")


# ---------------------------------------------------------------------
# 5. Точка входа (CLI)
# ---------------------------------------------------------------------
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Export YOLO model to ONNX with custom RPN ops (DecBBox, Filter, Sort, NMS).")
    parser.add_argument(
        "--weights",
        type=str,
        required=True,
        help="models/best.pt"
    )
    parser.add_argument(
        "--onnx-out",
        type=str,
        default=None,
        help="yolov11_onnx"
    )
    parser.add_argument(
        "--opset",
        type=int,
        default=13,
        help="ONNX opset version (по умолчанию 13)."
    )
    args = parser.parse_args()

    export_model(
        weights_path=args.weights,
        onnx_path=args.onnx_out,
        opset=args.opset
    )
