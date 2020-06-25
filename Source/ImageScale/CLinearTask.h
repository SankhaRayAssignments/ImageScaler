#pragma once
#include "itask.h"
#include "math_helper.h"

class CLinearTaskDataIn
{
private:
    const unsigned char** bi_high_rezied;
    const unsigned char** bi_low_rezied;
    unsigned char** dest_data;
    int start_height, end_height, w_dest;
    float h3;

    CLinearTaskDataIn(const unsigned char** bi_high_rezied, const unsigned char** bi_low_rezied,
        unsigned char** dest_data,
        int start_height, int end_height, int w_dest,
        float h3)
    {
        this->bi_high_rezied = bi_high_rezied;
        this->bi_low_rezied = bi_low_rezied;
        this->dest_data = dest_data;
        this->start_height = start_height;
        this->end_height = end_height;
        this->w_dest = w_dest;
        this->h3 = h3;
    }

    friend class CLinearTask;
};

class CLinearTaskResultOut
{
private:
    int result;
    friend class CLinearTask;
};

typedef void (*linearInterpolate)(const unsigned char**bi_high_rezied, const unsigned char**bi_low_rezied, unsigned char**dest_data, int x, int y, float h3);

class CLinearTask : public ITask
{
public:
    CLinearTask(unsigned int taskId, linearInterpolate func,
        const unsigned char** bi_high_rezied, const unsigned char** bi_low_rezied,
        unsigned char** dest_data,
        int start_height, int end_height, int w_dest,
        float h3);

    ~CLinearTask(void);

    void execute();

private:
    linearInterpolate mfunc;
    void printResult();
};
