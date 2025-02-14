#ifndef SPEED_MEASUREMENT_H
#define SPEED_MEASUREMENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tracking.h"

#define MAX_SPEED_TRACKS MAX_TRACKS
#define FPS 30.0f             //частота кадров
#define PIXEL_TO_METER 0.01f    //коэффициент преобразования пикселей в метры

typedef struct {
    int track_id;
    float prev_center_x;
    float prev_center_y;
    float speed_kmh;   //рассчитанная скорость в км/ч
    int valid;         
} SpeedInfo;

void speed_measurement_init(void);

void update_speed_measurements(Track *tracks, int num_tracks);

float get_speed_for_track(int track_id);

#ifdef __cplusplus
}
#endif

#endif
