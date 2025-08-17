#include "vector.h"

#include <math.h>
#include <stdlib.h>

#include "utils.h"

Vector2 add_vector(Vector2 a, Vector2 b) {
    Vector2 add = {a.x + b.x, a.y + b.y};
    return add;
}

Vector2 subtract_vector(Vector2 a, Vector2 b) {
    Vector2 subtract = {
        a.x - b.x,
        a.y - b.y,
    };
    return subtract;
}

Vector2 scale_vector(double scale, Vector2 a) {
    Vector2 scale_v = {a.x * scale, a.y * scale};
    return scale_v;
}

Vector2 div_vector(double div, Vector2 a) {
    Vector2 div_v = {a.x / div, a.y / div};
    return div_v;
}

double mag_vector(Vector2 vec) {
    double mag_v = sqrt(vec.x * vec.x + vec.y * vec.y);
    return mag_v;
}

Vector2 rand_vec() {
    Vector2 rand_v = {(float)rand() / RAND_MAX + 0.3,
                      (float)rand() / RAND_MAX + 0.3};
    return rand_v;
}

float eucld_dist(Vector2 a, Vector2 b) {
    Vector2 sub = subtract_vector(a, b);
    float eucl_dist_r = sqrtf(sqr(sub.x) + sqr(sub.y));
    return eucl_dist_r;
}