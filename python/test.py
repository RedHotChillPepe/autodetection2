import onnx
import numpy as np
import onnxruntime as ort

# Загружаем модель ONNX
model_path = "best.onnx"  # Замените на свой путь к файлу
model = onnx.load(model_path)

# Проверяем выходной слой
for output in model.graph.output:
    print(f"Output Name: {output.name}")
    print(f"Output Shape: {[dim.dim_value for dim in output.type.tensor_type.shape.dim]}")
    print(f"Data Type: {output.type.tensor_type.elem_type}")

# Создаем сессию ONNX Runtime
session = ort.InferenceSession(model_path)

# Проверяем количество классов (обычно последний размер выхода)
output_info = session.get_outputs()[0]
num_classes = output_info.shape[-1]
print(f"Количество классов: {num_classes}")