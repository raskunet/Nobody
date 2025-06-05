#ifndef BODY_H
#define BODY_H

#include "raylib.h"

#define MAX_BODIES (1 << 14)

typedef struct BODY {
    Vector3 position;
    Vector3 velocity;
    Vector3 acceleration;
    Vector3 prev_accel;
    double mass;
    Color color;
} Body;

Body rand_body();

Body* load_values_from_file(char*);

Body* body_init();

Body* init_cluster_bodies();

Body* init_colliding_galaxies();

void compute_body_force(Body*);

void update_body_positon(Body*);

void update_body_velocity(Body*);

void calculate_net_force(Body*, Body*);

Body* add(Body* A, Body* B);

#endif /* body.h */