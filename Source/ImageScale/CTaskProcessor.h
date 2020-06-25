#pragma once
#include <windows.h>
#include <process.h>
#include <vector>
#include "ITask.h"
using namespace std;

// State Machine
class IState
{
public:
    virtual bool submitTask()    =0;
    virtual bool startProcess()  =0;
    virtual bool clearAllTask()  =0;
};

// The Task Processor, it will invoke threads to perform submitted jobs
class CTaskProcessor
{
private:
    IState *STATUS_NO_TASK;
    IState *STATUS_IS_IN_PROGRESS;
    IState *STATUS_READY;
    IState *STATUS_COMPLETED;

public:
    CTaskProcessor(void);
    ~CTaskProcessor(void);

    void submitTask(ITask *task);
    void startProcess();
    void clearAllTask();

    int maxTaskCount();

private:
    IState *mStatus;
    vector<ITask*> mTaskList;
    vector<HANDLE> mThreadHandle;
};
