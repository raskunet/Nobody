#include "vector.h"

Vector3 add_vector(Vector3 a, Vector3 b) {
    Vector3 add = {a.x + b.x, a.y + b.y, a.z + b.z};
    return add;
}

Vector3 subtract_vector(Vector3 a, Vector3 b) {
    Vector3 subtract = {a.x - b.x, a.y - b.y, a.z - b.z};
    return subtract;
}

Vector3 scale_vector(double scale, Vector3 a) {
    Vector3 scale_v = {a.x * scale, a.y * scale, a.z * scale};
    return scale_v;
}

Vector3 div_vector(double div, Vector3 a) {
    Vector3 div_v = {a.x / div, a.y / div, a.z / div};
    return div_v;
}

double mag_vector(Vector3 vec) {
    double mag_v = sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    return mag_v;
}
Vector3 rand_vec() {
    Vector3 rand_v = {(float)rand() / RAND_MAX, (float)rand() / RAND_MAX};
    return rand_v;
}