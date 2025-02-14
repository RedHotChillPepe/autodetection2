#include "speed_measurement.h"
#include <math.h>
#include <string.h>

static SpeedInfo speed_info_array[MAX_SPEED_TRACKS];

void speed_measurement_init(void) {
    memset(speed_info_array, 0, sizeof(speed_info_array));
}

void update_speed_measurements(Track *tracks, int num_tracks) {
    for (int i = 0; i < num_tracks; i++) {
        int id = tracks[i].id;
        SpeedInfo *info = NULL;
        for (int j = 0; j < MAX_SPEED_TRACKS; j++) {
            if (speed_info_array[j].track_id == id) {
                info = &speed_info_array[j];
                break;
            }
        }
        if (!info) {
            for (int j = 0; j < MAX_SPEED_TRACKS; j++) {
                if (speed_info_array[j].valid == 0) {
                    info = &speed_info_array[j];
                    info->track_id = id;
                    break;
                }
            }
        }
        if (!info)
            continue;

        if (!info->valid) {
            info->prev_center_x = tracks[i].center_x;
            info->prev_center_y = tracks[i].center_y;
            info->speed_kmh = 0.0f;
            info->valid = 1;
        } else {
            //Вычисляем смещение центра между кадрами
            float dx = tracks[i].center_x - info->prev_center_x;
            float dy = tracks[i].center_y - info->prev_center_y;
            float dist_pixels = sqrtf(dx * dx + dy * dy);
            //Преобразуем в метры и затем в км/ч
            float speed_m_s = dist_pixels * PIXEL_TO_METER * FPS;
            info->speed_kmh = speed_m_s * 3.6f;
            //Обновляем предыдущие координаты
            info->prev_center_x = tracks[i].center_x;
            info->prev_center_y = tracks[i].center_y;
        }
    }
}

float get_speed_for_track(int track_id) {
    for (int i = 0; i < MAX_SPEED_TRACKS; i++) {
        if (speed_info_array[i].track_id == track_id && speed_info_array[i].valid) {
            return speed_info_array[i].speed_kmh;
        }
    }
    return 0.0f;
}
