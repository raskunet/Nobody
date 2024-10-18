#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raylib.h"
#include "vector.h"
static double G_CONST = 0.05;
static double dt = 1;  // Days in year
static double t_end = 365 * 10;
static double t = 0;
#define MAX_BODIES 1024

#define degToRad(angle_in_degrees) ((angleInDegrees) * PI / 180.0)
#define radToDeg(angle_in_radians) ((angleInRadians) * 180.0 / PI)

#define arrSize(array) ((sizeof(array) / sizeof((array)[0])))

typedef struct BODY {
    Vector3 position;
    Vector3 velocity;
    double mass;
    union SHAPE {
        Color color;
        Image image;
    } shape;
} Body;

static Body rand_body() {
    Body rand_b;
    rand_b.position = rand_vec();
    rand_b.velocity = rand_vec();
    rand_b.mass = (float)rand() / RAND_MAX;
    rand_b.shape.color = WHITE;
    return rand_b;
}

static Body* load_values_from_file(char* filename) {
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

static double sqr(double x) { return x * x; }

static double eucld_dist(Vector3 a, Vector3 b) {
    Vector3 sub = subtract_vector(a, b);
    double eucl_dist_r = sqrt(sqr(sub.x) + sqr(sub.y) + sqr(sub.z));
    return eucl_dist_r;
}