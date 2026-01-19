#ifndef TOOLBOX_H
#define TOOLBOX_H

#include "math.h"
#define PI 3.141592653589793

float Abs(float);
float Max(float, float);
float Min(float, float);
float LimitToInterval(float, float, float);
float RadianToDegree(float);
float DegreeToRadian(float);
void getBytesFromFloat(unsigned char*, int, float);
float getFloatFromBytes(const unsigned char *p, int index);
void getBytesFromInt32(unsigned char*, int, long);
void getBytesFromDouble(unsigned char*, int, double);

#endif /* TOOLBOX_H */