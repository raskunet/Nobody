#include <math.h>
#include <stdlib.h>

#include "raylib.h"
#ifndef VECTOR_H
#define VECTOR_H

Vector3 add_vector(Vector3 a, Vector3 b);

Vector3 subtract_vector(Vector3 a, Vector3 b);

Vector3 scale_vector(double scale, Vector3 a);

Vector3 div_vector(double div, Vector3 a);

double mag_vector(Vector3 vec);

Vector3 rand_vec();

#endif  // VECTOR_H