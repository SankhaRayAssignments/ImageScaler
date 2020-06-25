#include "StdAfx.h"
#include <stdio.h>
#include "CLinearTask.h"

CLinearTask::CLinearTask(unsigned int taskId, linearInterpolate func,
        const unsigned char** bi_high_rezied, const unsigned char** bi_low_rezied,
        unsigned char** dest_data,
        int start_height, int end_height, int w_dest,
        float h3):ITask(taskId)
{
    this->mfunc = func; // Save the function pointer for future use

    CLinearTaskDataIn *dataIn = new CLinearTaskDataIn(bi_high_rezied, bi_low_rezied,
                                                      dest_data,
                                                      start_height, end_height, w_dest,
                                                      h3);

    mTaskDataIn = (void*)dataIn;
}

CLinearTask::~CLinearTask(void)
{
    CLinearTaskDataIn *dataIn = (CLinearTaskDataIn *)mTaskDataIn;
    if ( dataIn )
        delete dataIn;
    dataIn = NULL;

    CLinearTaskResultOut *dataOut = (CLinearTaskResultOut *)mTaskResultOut;
    if ( dataOut )
        delete dataOut;
    dataOut = NULL;
}

void CLinearTask::execute()
{
    CLinearTaskDataIn *taskdata = (CLinearTaskDataIn *)mTaskDataIn;

    for ( int y = taskdata->start_height; y < taskdata->end_height; y++)
    {
        for (int x=0; x<taskdata->w_dest; x++) {
            this->mfunc(taskdata->bi_high_rezied,
                taskdata->bi_low_rezied,
                taskdata->dest_data,
                x, y,
                taskdata->h3);
        }
    }

    //printResult();
}

void CLinearTask::printResult()
{
    printf("Completed TaskId[%d]\n", mTaskId);
}
