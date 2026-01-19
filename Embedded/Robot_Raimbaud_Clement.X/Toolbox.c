#include "Toolbox.h"
float Abs(float value)
{
    if (value >= 0)
    return value;
    else return -value;
}

float Max(float value, float value2)
{
    if (value > value2)
    return value;
    else
    return value2;
}

float Min(float value, float value2)
{
    if (value < value2)
    return value;
    else
    return value2;
}

float LimitToInterval(float value, float lowLimit, float highLimit)
{
    if (value > highLimit)
    value = highLimit;
    else if (value < lowLimit)
    value = lowLimit;
    return value;
}

float RadianToDegree(float value)
{
    return value / PI * 180.0;
}

float DegreeToRadian(float value)
{
return value * PI / 180.0;
}

void getBytesFromFloat(unsigned char *p, int index, float f)
{
    int i;
    unsigned char *f_ptr = (unsigned char*)&f;
    for (i = 0; i < 4; i++)
        p[index + i] = f_ptr[i];
}

float getFloatFromBytes(const unsigned char *p, int index)
{
    float f;
    unsigned char *f_ptr = (unsigned char*)&f;

    for (int i = 0; i < 4; i++)
        f_ptr[i] = p[index + i];

    return f;
}

void getBytesFromInt32(unsigned char *p, int index, long in)
{
    int i;
    unsigned char *f_ptr = (unsigned char*)&in;
    for (i = 0; i < 4; i++)
        p[index + i] = f_ptr[3-i];
}


void getBytesFromDouble(unsigned char *p, int index, double d)
{
    int i;
    unsigned char *f_ptr = (unsigned char*)&d;
    for (i = 0; i < 8; i++)
        p[index + i] = f_ptr[i];
}

