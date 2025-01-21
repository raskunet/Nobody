#include "raylib.h"

static double G_CONST = 1;
static double dt = 0.25; 
static double t_end = 365 * 10;
static double t = 0;

#define degToRad(angle_in_degrees) ((angleInDegrees) * PI / 180.0)
#define radToDeg(angle_in_radians) ((angleInRadians) * 180.0 / PI)

#define arrSize(array) ((sizeof(array) / sizeof((array)[0])))

static double sqr(double x) { return x * x; }

