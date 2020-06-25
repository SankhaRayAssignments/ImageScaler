#pragma once
#include "itask.h"
#include "math_helper.h"

class CBilinearTaskDataIn
{
private:
    const unsigned char**  source_data;
    int w_source, h_source;
    int start_height, end_height;
    int w_dest;
    unsigned char**  dest_data;
    Point2D destPoint;
    float scaleX, scaleY;

    CBilinearTaskDataIn(const unsigned char**source_data, int w_source, int h_source, int start_height, int end_height, int w_dest, unsigned char**dest_data, float scaleX, float scaleY)
    {
        this->source_data = source_data;
        this->w_source    = w_source;
        this->h_source    = h_source;
        this->start_height= start_height;
        this->end_height  = end_height;
        this->w_dest        = w_dest;
        this->dest_data   = dest_data;
        this->scaleX      = scaleX;
        this->scaleY      = scaleY;
    }

    friend class CBilinearTask;
};

class CBilinearTaskResultOut
{
private:
    int result;
    friend class CBilinearTask;
};

typedef void (*bilinearInterpolation)(const unsigned char**  source_data, int w_source, int h_source, unsigned char**  dest_data, Point2D& destPoint, float scaleX, float scaleY);

class CBilinearTask : public ITask
{
public:
    CBilinearTask(unsigned int taskId, bilinearInterpolation func,
        const unsigned char** source_data, int w_source, int h_source,
        int start_height, int end_height, int w_dest,
        unsigned char**  dest_data,
        float scaleX, float scaleY);

    ~CBilinearTask(void);

    void execute();

private:
    bilinearInterpolation mfunc;
    void printResult();
};
