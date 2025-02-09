#ifndef TRACKING_H
#define TRACKING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <math.h>

/* Структура для описания bounding box */
typedef struct {
    float x;
    float y;
    float w;
    float h;
} BBox;

/* Структура для описания трека объекта */
typedef struct {
    int id;
    BBox bbox;
    int lost_frames;
    float center_x;
    float center_y;
} Track;

/* Константы для трекинга */
#define MAX_TRACKS 100
#define DISTANCE_THRESHOLD 50.0f 
#define MAX_LOST_FRAMES 5

/* Глобальные переменные, определяемые в tracking.c */
extern Track tracks[MAX_TRACKS];
extern int track_count;
extern int next_track_id;

/* Прототипы функций трекинга */
float euclidean_distance(float x1, float y1, float x2, float y2);
void update_tracks(BBox *detections, int num_detections);
void print_tracks(void);

#ifdef __cplusplus
}
#endif

#endif /* TRACKING_H */