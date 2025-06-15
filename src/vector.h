#ifndef VECTOR_H
#define VECTOR_H

#include "raylib.h"

Vector3 add_vector(Vector3 a, Vector3 b);

Vector3 subtract_vector(Vector3 a, Vector3 b);

Vector3 scale_vector(double scale, Vector3 a);

Vector3 div_vector(double div, Vector3 a);

double mag_vector(Vector3 vec);

Vector3 rand_vec();

float eucld_dist(Vector3 a, Vector3 b);

#endif /* vector.h */