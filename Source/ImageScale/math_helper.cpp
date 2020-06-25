#pragma once
#include "StdAfx.h"
#include "math_helper.h"

// Point2D
Point2D::Point2D()
{
    x =  y = 0;
}
Point2D::Point2D(int xIn, int yIn)
{
    x = xIn;
    y = yIn;
}

Point2D Point2D::operator * (int value)
{
    Point2D outPoint;
    outPoint.x = this->x * value;
    outPoint.y = this->y * value;
    return outPoint;
}

// Point4D
Point4D::Point4D()
{
    x =  y = z = w = 0;
}
Point4D::Point4D(float xIn, float yIn, float zIn, float wIn)
{
    x = xIn;
    y = yIn;
    z = zIn;
    w = wIn;
}

Point4D::Point4D(void *ptr)
{
    if ( ptr == 0 )
        this->x = this->y = this->z = this->w = 0;

    x = ((unsigned char*)ptr)[0];
    y = ((unsigned char*)ptr)[1];
    z = ((unsigned char*)ptr)[2];
    w = ((unsigned char*)ptr)[3];
}

Point4D::Point4D(const unsigned char**dataIn, int x, int y)
{
    const unsigned char* row = dataIn[y];
    const unsigned char* ptr = &(row[x*4]);

    this->x = ptr [0];
    this->y = ptr [1];
    this->z = ptr [2];
    this->w = ptr [3];
}

Point4D Point4D::operator * (float value)
{
    Point4D outPoint;
    outPoint.x = this->x * value;
    outPoint.y = this->y * value;
    outPoint.z = this->z * value;
    outPoint.w = this->w * value;
    return outPoint;
}

Point4D Point4D::operator / (float value)
{
    if ( value == 0.0f )
        return *this;
    Point4D outPoint;
    outPoint.x = this->x / value;
    outPoint.y = this->y / value;
    outPoint.z = this->z / value;
    outPoint.w = this->w / value;
    return outPoint;
}

Point4D Point4D::operator + (Point4D& pointIn)
{
    Point4D outPoint;
    outPoint.x = this->x + pointIn.x;
    outPoint.y = this->y + pointIn.y;
    outPoint.z = this->z + pointIn.z;
    outPoint.w = this->w + pointIn.w;
    return outPoint;
}

Point4D Point4D::operator - (Point4D& pointIn)
{
    Point4D outPoint;
    outPoint.x = this->x - pointIn.x;
    outPoint.y = this->y - pointIn.y;
    outPoint.z = this->z - pointIn.z;
    outPoint.w = this->w - pointIn.w;
    return outPoint;
}
