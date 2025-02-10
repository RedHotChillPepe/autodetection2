import os
import random
import shutil
from glob import glob

def split_dataset(data_dir, output_dir, split_ratios):

    # Создание выходных директорий для train, val, test
    for split in ["train", "val", "test"]:
        os.makedirs(os.path.join(output_dir, split, "images"), exist_ok=True)
        os.makedirs(os.path.join(output_dir, split, "labels"), exist_ok=True)

    # Получение списка всех изображений и аннотаций
    image_files = glob(os.path.join(data_dir, "*.jpg")) + glob(os.path.join(data_dir, "*.png"))
    annotation_files = [f.replace(".jpg", ".txt").replace(".png", ".txt") for f in image_files]

    # Перемешивание и разделение датасета на train, val, test
    data_pairs = list(zip(image_files, annotation_files))
    random.shuffle(data_pairs)

    train_ratio, val_ratio, test_ratio = split_ratios
    train_split = int(len(data_pairs) * train_ratio)
    val_split = train_split + int(len(data_pairs) * val_ratio)

    train_data = data_pairs[:train_split]
    val_data = data_pairs[train_split:val_split]
    test_data = data_pairs[val_split:]

    # Функция для копирования файлов в соответствующие папки
    def copy_data(data, split):
        for img_file, ann_file in data:
            shutil.copy(img_file, os.path.join(output_dir, split, "images", os.path.basename(img_file)))
            shutil.copy(ann_file, os.path.join(output_dir, split, "labels", os.path.basename(ann_file)))

    # Копирование файлов в соответствующие папки
    copy_data(train_data, "train")
    copy_data(val_data, "val")
    copy_data(test_data, "test")

    print("Датасет успешно разделен на train, val и test")

# Пример использования
if __name__ == "__main__":
    data_directory = "D:\Projects\Баст IT\ПО\Автодетекция\DB\Frames"
    output_directory = "D:\Projects\Баст IT\ПО\Автодетекция\DB\Train"
    split_ratios = (0.7, 0.2, 0.1)  # Доли для train, val, test

    split_dataset(data_directory, output_directory, split_ratios)
