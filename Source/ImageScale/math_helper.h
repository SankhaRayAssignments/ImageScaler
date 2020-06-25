#pragma once

class Point2D
{
public:
    int x,y;
    Point2D();
    Point2D(int xIn, int yIn);
    Point2D operator * (int value);
};

class Point4D
{
public:
    float x,y,z,w;

    Point4D();
    Point4D(float xIn, float yIn, float zIn, float wIn);
    Point4D(void *ptr);
    Point4D(const unsigned char**dataIn, int x, int y);
    Point4D operator * (float value);
    Point4D operator / (float value);
    Point4D operator + (Point4D& pointIn);
    Point4D operator - (Point4D& pointIn);
};

#define min(x,y) (x)<(y)?(x):(y)
#define max(x,y) (x)>(y)?(x):(y)
