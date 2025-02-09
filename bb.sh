#!/bin/bash

# Установка переменных окружения
export GSF_VPSS_YUV_SUPP=1
export GSF_OSD_FREETYPE_SUPP=1

echo "Переменные окружения установлены."

# Очистка и сборка проекта
if make clean && make ARCH=3519dv5; then
    echo "Сборка прошла успешно."
else
    echo "Ошибка при сборке." >&2
    exit 1
fi

# Копирование файла
destination="/home/admin1/nfs/test/test3"
if cp test3 "$destination"; then
    echo "Файл успешно скопирован в $destination."
else
    echo "Ошибка при копировании файла." >&2
    exit 1
fi

