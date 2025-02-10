"""
inspect_onnx.py

Скрипт для проверки и «реверс-инжиниринга» ONNX-модели.
Он выводит много полезной информации:
- Общую (IR версию, opset, domain).
- Список входов/выходов (имена, формы, типы).
- Перечень узлов (op_type, домен, входы/выходы, атрибуты).
- Ищет "кастомные" операторы (не стандартного домена).
- Опционально запускает shape_inference.

Также сохраняет всю информацию в txt-файл, где название файла
соответствует названию вашей ONNX-модели.
Пример:
  python inspect_onnx.py --model /path/to/best.onnx
  => сохранит результат в "best.txt"
"""

import argparse
import os
import onnx
from onnx import shape_inference

def main():
    parser = argparse.ArgumentParser(
        description="Inspect an ONNX model and print/save detailed info."
    )
    parser.add_argument(
        "--model",
        type=str,
        required=True,
        help="Path to the ONNX model file (e.g., model.onnx)."
    )
    parser.add_argument(
        "--infer-shapes",
        action="store_true",
        help="Whether to run shape inference on the model (may fail on custom ops)."
    )
    args = parser.parse_args()

    # Определим имя txt-файла:
    model_path = args.model
    base_name = os.path.splitext(os.path.basename(model_path))[0]
    txt_filename = f"{base_name}.txt"

    # Вместо print(...) будем собирать строки в список, чтобы потом записать в txt.
    output_lines = []

    def log(msg=""):
        # Одновременно выводим в консоль и копим в список строк.
        print(msg)
        output_lines.append(msg)

    log(f"[INFO] Loading ONNX model from: {model_path}")
    model = onnx.load(model_path)

    # 1) Проверка модели на корректность
    try:
        onnx.checker.check_model(model)
        log("[INFO] The model is valid according to onnx.checker.")
    except onnx.checker.ValidationError as e:
        log(f"[WARNING] Model is not strictly valid: {e}")

    # 2) Общая информация
    ir_version = model.ir_version
    opset_imports = model.opset_import
    producer_name = model.producer_name
    producer_ver = model.producer_version
    domain = model.domain
    model_version = model.model_version
    doc_string = model.doc_string

    log("\n--- General Info ---")
    log(f"IR Version: {ir_version}")
    log("Opset imports:")
    for oi in opset_imports:
        dom = oi.domain if oi.domain else "ai.onnx"
        log(f"  - domain='{dom}', version={oi.version}")

    log(f"Producer name: {producer_name}")
    log(f"Producer version: {producer_ver}")
    log(f"Domain: {domain}")
    log(f"Model version: {model_version}")
    log(f"Doc string: {doc_string}")

    # 3) Вывод входов и выходов
    graph = model.graph
    log("\n--- Model Inputs ---")
    for inp in graph.input:
        shape_dims = []
        if inp.type.tensor_type.shape and inp.type.tensor_type.shape.dim:
            for d in inp.type.tensor_type.shape.dim:
                if d.dim_param:
                    shape_dims.append(d.dim_param)
                else:
                    shape_dims.append(str(d.dim_value))
        elem_type = inp.type.tensor_type.elem_type
        log(f"  name='{inp.name}', type={elem_type}, shape={shape_dims}")

    log("\n--- Model Outputs ---")
    for outp in graph.output:
        shape_dims = []
        if outp.type.tensor_type.shape and outp.type.tensor_type.shape.dim:
            for d in outp.type.tensor_type.shape.dim:
                if d.dim_param:
                    shape_dims.append(d.dim_param)
                else:
                    shape_dims.append(str(d.dim_value))
        elem_type = outp.type.tensor_type.elem_type
        log(f"  name='{outp.name}', type={elem_type}, shape={shape_dims}")

    # 4) Опционально, shape inference
    if args.infer_shapes:
        log("\n[INFO] Running shape_inference...")
        try:
            model = shape_inference.infer_shapes(model)
            log("[INFO] Shape inference completed successfully.")
        except Exception as e:
            log(f"[WARNING] shape_inference failed: {e}")
        # Обновим graph после shape_inference
        graph = model.graph

    # 5) Список узлов
    log("\n--- Nodes (layers) in the graph ---")
    for i, node in enumerate(graph.node):
        op_type = node.op_type
        dom = node.domain
        log(f"[Node {i}] op_type='{op_type}', domain='{dom}'")
        log(f"   inputs= {list(node.input)}")
        log(f"   outputs={list(node.output)}")
        if node.attribute:
            log("   attrs:")
            for att in node.attribute:
                log(f"     - {att.name} = {att}")

    # 6) Поиск кастомных операторов
    log("\n--- Checking for custom ops (non-standard domain or suspicious op_type) ---")
    custom_ops = []
    for i, node in enumerate(graph.node):
        dom = node.domain
        # Стандартные домены: '' или 'ai.onnx', 'ai.onnx.ml'
        if dom not in ("", "ai.onnx", "ai.onnx.ml"):
            custom_ops.append((i, node.op_type, dom))
        # Или ищем в op_type подстроку 'custom_ops'
        if "custom_ops" in node.op_type.lower():
            custom_ops.append((i, node.op_type, dom))

    if custom_ops:
        log("Found potential custom operators:")
        for (idx, op_type, d) in custom_ops:
            log(f"  Node {idx}: op_type='{op_type}', domain='{d}'")
    else:
        log("No obvious custom operators found.")

    log("\n--- Done. ---")

    # 7) Сохраним весь вывод в txt-файл
    with open(txt_filename, "w", encoding="utf-8") as f:
        for line in output_lines:
            f.write(line + "\n")

    print(f"[INFO] Detailed info saved to '{txt_filename}'")

if __name__ == "__main__":
    main()