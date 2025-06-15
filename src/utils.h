#ifndef UTILS_H
#define UTILS_H

#define degToRad(angle_in_degrees) ((angleInDegrees) * PI / 180.0)
#define radToDeg(angle_in_radians) ((angleInRadians) * 180.0 / PI)
#define arrSize(array) ((sizeof(array) / sizeof((array)[0])))

float sqr(float);


#endif  /* utils.h */