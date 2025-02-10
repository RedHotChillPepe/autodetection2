# import onnx
# from onnx import version_converter, helper
#
# original_model = onnx.load("best_simplified.onnx")
# converted_model = version_converter.convert_version(original_model, 12)
# onnx.save(converted_model, "best_v12.onnx")

import onnx

model = onnx.load("best.onnx")

model.ir_version = 7  # Задайте версию IR, подходящую для вашей модели
for opset in model.opset_import:
    opset.version = 12

onnx.save(model, "best_v12_manual.onnx")