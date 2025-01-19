#include "body.h"

#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "raylib.h"
#include "utils.c"
#include "vector.h"

Body rand_body() {
    Body rand_b;
    rand_b.position = rand_vec();
    rand_b.velocity = rand_vec();
    rand_b.mass = (float)rand() / RAND_MAX;
    rand_b.shape.color = WHITE;
    rand_b.prev_accel = (Vector3){0.0f, 0.0f, 0.0f};
    return rand_b;
}

Body* load_values_from_file(char* filename) {
    if (!FileExists(filename)) {
        return NULL;
    }
    Body* body = (Body*)MemAlloc(MAX_BODIES * sizeof(Body));
    char* fileDat = LoadFileText(filename);
    int* counter = MemAlloc(sizeof(int));
    const char** lines = TextSplit(fileDat, '\n', counter);

    char** alloc_lines = (char**)MemAlloc(*counter * sizeof(char*));
    for (int i = 0; i < *counter; i++) {
        alloc_lines[i] = (char*)MemAlloc(strlen(lines[i]) * sizeof(char) + 1);
        strcpy(alloc_lines[i], lines[i]);
    }

    for (int i = 0; i < *counter; i++) {
        int* count = MemAlloc(sizeof(int));
        const char** line = TextSplit(alloc_lines[i], ' ', count);
        // printf("x=%s,y=%s,vx=%s,vy=%s,m=%s count:%d\n", line[0], line[1],
        //        line[3], line[4], line[6], *count);
        if (*count == 7) {
            body[i].position.x = strtod(line[0], NULL);
            body[i].position.y = strtod(line[1], NULL);
            body[i].position.z = strtod(line[2], NULL);

            body[i].velocity.x = strtod(line[3], NULL);
            body[i].velocity.y = strtod(line[4], NULL);
            body[i].velocity.z = strtod(line[5], NULL);

            body[i].acceleration = (Vector3){0, 0, 0};
            body[i].prev_accel = (Vector3){0, 0, 0};

            body[i].mass = strtod(line[6], NULL);
            body[i].shape.color = YELLOW;
        }
        MemFree(count);
    }

    for (int i = 0; i < *counter; i++) {
        MemFree(alloc_lines[i]);
    }
    MemFree(alloc_lines);
    MemFree(counter);
    return body;
}

Body* body_init() {
    srand(time(NULL));
    Body* body_arr = (Body*)MemAlloc(MAX_BODIES * sizeof(Body));
    for (size_t i = 0; i < MAX_BODIES; i++) {
        body_arr[i] = rand_body();
    }

    Vector3 v_sum = {0}, p_sum = {0};
    double mass_sum = 0;
    for (size_t i = 0; i < MAX_BODIES; i++) {
        v_sum = add_vector(v_sum, body_arr[i].velocity);
        p_sum = add_vector(p_sum, body_arr[i].position);
        mass_sum += body_arr[i].mass;
    }

    Vector3 v_avg = div_vector(mass_sum, v_sum),
            p_avg = div_vector(mass_sum, p_sum);

    for (size_t i = 0; i < MAX_BODIES; i++) {
        body_arr[i].velocity = subtract_vector(body_arr[i].velocity, v_avg);
        body_arr[i].position = subtract_vector(body_arr[i].position, p_avg);
    }

    double max_r = -DBL_MAX;

    for (size_t i = 0; i < MAX_BODIES; i++) {
        double dist_r = mag_vector(body_arr[i].position);
        if (dist_r > max_r) {
            max_r = dist_r;
        }
    }

    for (size_t i = 0; i < MAX_BODIES; i++) {
        body_arr[i].position = div_vector(max_r, body_arr[i].position);
    }

    return body_arr;
}

Body* init_cluster_bodies() {
    srand(time(NULL));
    Body* body_arr = (Body*)MemAlloc(MAX_BODIES * sizeof(Body));
    // set origin
    body_arr[0].mass = 1e8;
    body_arr[0].position = (Vector3){0, 0, 0};
    body_arr[0].velocity = (Vector3){
        0,
        0,
        0,
    };
    body_arr[0].shape.color = YELLOW;

    float nominal_mass = 1;
    double total_mass = body_arr[0].mass;
    for (int i = 1; i < MAX_BODIES; i++) {
        body_arr[i].mass = (rand() / (float)RAND_MAX) * nominal_mass;
        total_mass += body_arr[i].mass;
        body_arr[i].shape.color = WHITE;
    }
    for (int i = 1; i < MAX_BODIES; i++) {
        double r = 10000 * sqrt((rand()) / (float)RAND_MAX);
        double theta = (rand() / (double)RAND_MAX) * 2 * PI;

        body_arr[i].position.x = cos(theta) * r;
        body_arr[i].position.y = sin(theta) * r;
        body_arr[i].position.z = 0.0;

        r = sqrtf(G_CONST * total_mass / r) * (rand() / (double)RAND_MAX + 3) /
            4;
        theta += M_PI / 2 + (rand() / (double)RAND_MAX - 0.5) * M_PI / 2;

        body_arr[i].velocity.x = cos(theta) * r;
        body_arr[i].velocity.y = sin(theta) * r;
        body_arr[i].velocity.z = 0.0;

        //(rand() / (float)RAND_MAX - 0.5) * 10000.0;)
        // body_arr[i].velocity.x = (rand() / (float)RAND_MAX - 0.5) * 100.0f;
        // body_arr[i].velocity.y = (rand() / (float)RAND_MAX - 0.5) * 100.0f;
        // body_arr[i].velocity.z = 0.0;
    }
    return body_arr;
}

void compute_body_force(Body* body_t) {
    double ESP2 = 1.0e-2f;

    for (size_t i = 0; i < MAX_BODIES; i++) {
        body_t[i].prev_accel = body_t[i].acceleration;
        body_t[i].acceleration = (Vector3){0, 0, 0};

        for (size_t j = 0; j < MAX_BODIES; j++) {
            if (j != i) {
                double mass = body_t[j].mass;
                Vector3 dist =
                    subtract_vector(body_t[j].position, body_t[i].position);

                double mag_r = dist.x * dist.x + dist.y * dist.y + ESP2;

                float distSixth = mag_r * mag_r * mag_r;

                float invDistCube = 1.0f * (1 / sqrt(distSixth));

                float f = mass * invDistCube;

                body_t[i].acceleration.x += G_CONST * f * dist.x;
                body_t[i].acceleration.y += G_CONST * f * dist.y;
            }
        }
    }
}

void update_body_positon(Body* body_t) {
    float dt_half = dt * 0.5f;
    body_t->position = add_vector(
        body_t->position,
        add_vector(scale_vector(dt, body_t->velocity),
                   scale_vector(((dt * dt) * 0.5f), body_t->acceleration)));
}

void update_body_velocity(Body* body_t) {
    body_t->velocity = add_vector(
        body_t->velocity,
        scale_vector(dt * 0.5f,
                     add_vector(body_t->acceleration, body_t->prev_accel)));
}

Body* add(Body* A, Body* B) {
    /*
        m = m1 + m2
        x = ( x1m1 + x2m2 ) / m
        y = ( y1m1 + y2m2 ) / m
    */
    Body* acumBody = (Body*)MemAlloc(sizeof(Body));
    acumBody->mass = A->mass + B->mass;

    acumBody->position.x =
        (A->position.x * A->mass + B->position.x * B->mass) / acumBody->mass;

    acumBody->position.x =
        (A->position.y * A->mass + B->position.y * B->mass) / acumBody->mass;
    
    MemFree(A);
    return acumBody;
}