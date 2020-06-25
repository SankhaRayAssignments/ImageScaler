#pragma once

class ITask
{
public:
    ITask(unsigned int taskId)
    {
        mTaskId = taskId;
        mTaskDataIn = mTaskResultOut = NULL;
    }
    virtual ~ITask(void)
    {
    }

    virtual void execute() =0;                      // Sub Class Task should define

    void* getResult()
    {
        return mTaskResultOut;
    }

protected:
    void        *mTaskDataIn;
    void        *mTaskResultOut;
    unsigned int mTaskId;

    virtual void printResult(/*int taskId*/)    =0; // Sub Class Task should define
};
