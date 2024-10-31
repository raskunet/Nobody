#ifndef BODY_H
#define BODY_H

#include "raylib.h"

#define MAX_BODIES 2

typedef struct BODY {
    Vector3 position;
    Vector3 velocity;
    Vector3 acceleration;
    Vector3 prev_accel;
    double mass;
    union SHAPE {
        Color color;
        Image image;
    } shape;
} Body;

Body rand_body();

Body* load_values_from_file(char*);

Body* body_init();

Body* init_cluster_bodies();

void compute_body_force(Body*);

void update_body_positon(Body*);

#endif /* body.h */