#include "tracking.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

extern int get_detections(BBox *detections, int max_detections);

Track tracks[MAX_TRACKS];
int track_count = 0;
int next_track_id = 0;

float euclidean_distance(float x1, float y1, float x2, float y2) {
    return sqrtf((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

void update_tracks(BBox *detections, int num_detections) {
    int updated_tracks[MAX_TRACKS] = {0};
    for (int i = 0; i < num_detections; i++) {
        printf("DEBUG update_tracks: detection[%d].cid = %d\n", i, detections[i].cid);
    }

    for (int i = 0; i < num_detections; i++) {
        float det_center_x = detections[i].x + detections[i].w / 2.0f;
        float det_center_y = detections[i].y + detections[i].h / 2.0f;
        int best_track_idx = -1;
        float best_distance = DISTANCE_THRESHOLD;

        for (int j = 0; j < track_count; j++) {
            float distance = euclidean_distance(det_center_x, det_center_y,
                                                  tracks[j].center_x, tracks[j].center_y);
            if (distance < best_distance) {
                best_distance = distance;
                best_track_idx = j;
            }
        }

        if (best_track_idx != -1) {
            tracks[best_track_idx].bbox = detections[i];
            tracks[best_track_idx].center_x = det_center_x;
            tracks[best_track_idx].center_y = det_center_y;
            tracks[best_track_idx].lost_frames = 0;
            tracks[best_track_idx].cid = detections[i].cid;
            updated_tracks[best_track_idx] = 1;
        } else {
            if (track_count < MAX_TRACKS) {
                tracks[track_count].id = next_track_id++;
                tracks[track_count].bbox = detections[i];
                tracks[track_count].center_x = det_center_x;
                tracks[track_count].center_y = det_center_y;
                tracks[track_count].lost_frames = 0;
                tracks[track_count].cid = detections[i].cid;
                track_count++;
            }
        }
    }

    for (int j = 0; j < track_count; j++) {
        if (!updated_tracks[j]) {
            tracks[j].lost_frames++;
        }
    }

    for (int j = 0; j < track_count; ) {
        if (tracks[j].lost_frames > MAX_LOST_FRAMES) {
            tracks[j] = tracks[track_count - 1];
            track_count--;
        } else {
            j++;
        }
    }
}

void print_tracks(void) {
    for (int i = 0; i < track_count; i++) {
        printf("Track ID: %d, bbox: (%.1f, %.1f, %.1f, %.1f), lost_frames: %d\n",
               tracks[i].id,
               tracks[i].bbox.x, tracks[i].bbox.y,
               tracks[i].bbox.w, tracks[i].bbox.h,
               tracks[i].lost_frames);
    }
}

