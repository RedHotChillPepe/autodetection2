#!/bin/bash

# Установка переменных окружения
export GSF_VPSS_YUV_SUPP=1
export GSF_OSD_FREETYPE_SUPP=1

# Запуск в новом окне терминала
gnome-terminal -- bash -c "\
    echo 'Переменные окружения установлены.'; \
    make clean && make ARCH=3519dv5 && \
    echo 'Сборка прошла успешно.' || { echo 'Ошибка при сборке.' >&2; exit 1; }; \
    cp test3 /home/admin1/nfs//test/test3 && \
    echo 'Файл успешно скопирован в /home/admin1/nfs/test/test3.' || { echo 'Ошибка при копировании файла.' >&2; exit 1; }; \
    echo 'Нажмите Enter для выхода...'; read"
