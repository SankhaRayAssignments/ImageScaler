#include "StdAfx.h"
#include <stdio.h>
#include "CBilinearTask.h"

CBilinearTask::CBilinearTask(unsigned int taskId, bilinearInterpolation func,
    const unsigned char** source_data, int w_source, int h_source,
    int start_height, int end_height, int w_dest,
    unsigned char**  dest_data,
    float scaleX, float scaleY):ITask(taskId)
{
    this->mfunc = func; // Save the function pointer for future use
    
    CBilinearTaskDataIn *dataIn = new CBilinearTaskDataIn(source_data, w_source, h_source, start_height, end_height, w_dest, dest_data, scaleX, scaleY);

    mTaskDataIn = (void*)dataIn;
}

CBilinearTask::~CBilinearTask(void)
{
    CBilinearTaskDataIn *dataIn = (CBilinearTaskDataIn *)mTaskDataIn;
    if ( dataIn )
        delete dataIn;
    dataIn = NULL;

    CBilinearTaskResultOut *dataOut = (CBilinearTaskResultOut *)mTaskResultOut;
    if ( dataOut )
        delete dataOut;
    dataOut = NULL;
}

void CBilinearTask::execute()
{
    CBilinearTaskDataIn *taskdata = (CBilinearTaskDataIn *)mTaskDataIn;

    for ( int y = taskdata->start_height; y < taskdata->end_height; y++)
    {
        for (int x=0; x<taskdata->w_dest; x++) {
            this->mfunc(taskdata->source_data,
                taskdata->w_source, taskdata->h_source,
                taskdata->dest_data,
                Point2D(x,y),
                taskdata->scaleX, taskdata->scaleY);
        }
    }

    //printResult();
}

void CBilinearTask::printResult()
{
    printf("Completed TaskId[%d]\n", mTaskId);
}
