#ifndef VECTOR_H
#define VECTOR_H

#include "raylib.h"

Vector2 add_vector(Vector2 a, Vector2 b);

Vector2 subtract_vector(Vector2 a, Vector2 b);

Vector2 scale_vector(double scale, Vector2 a);

Vector2 div_vector(double div, Vector2 a);

double mag_vector(Vector2 vec);

Vector2 rand_vec();

float eucld_dist(Vector2 a, Vector2 b);

#endif /* vector.h */