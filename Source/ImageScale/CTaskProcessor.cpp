#include "StdAfx.h"
#include "CTaskProcessor.h"

/*********************************************************************/
// State Machines
class NoTaskState : public IState
{
private:
    CTaskProcessor* mTaskProcessor;
public:
    NoTaskState(CTaskProcessor& processor)
    {
        mTaskProcessor = &processor;
    }
    bool submitTask()
    {
        return true;
    }
    bool startProcess()
    {
        printf("No Tasks Found. Submit Tasks first\n");
        return false;
    }
    bool clearAllTask()
    {
        return true;
    }
};

class IsInProgressState : public IState
{
private:
    CTaskProcessor* mTaskProcessor;
public:
    IsInProgressState(CTaskProcessor& processor)
    {
        mTaskProcessor = &processor;
    }
    bool submitTask()
    {
        printf("Task Proccessor is Busy, Wait till it finishes execution\n");
        return false;
    }
    bool startProcess()
    {
        return true;
    }
    bool clearAllTask()
    {
        printf("Task Proccessor is Busy, Wait till it finishes execution\n");
        return false;
    }
};

class TaskReadyState : public IState
{
private:
    CTaskProcessor* mTaskProcessor;
public:
    TaskReadyState(CTaskProcessor& processor)
    {
        mTaskProcessor = &processor;
    }
    bool submitTask()
    {
        return true;
    }
    bool startProcess()
    {
        return true;
    }
    bool clearAllTask()
    {
        return true;
    }
};

class TaskCompletedState : public IState
{
private:
    CTaskProcessor* mTaskProcessor;
public:
    TaskCompletedState(CTaskProcessor& processor)
    {
        mTaskProcessor = &processor;
    }
    bool submitTask()
    {
        printf("Task Proccessor is Completed, Clear Task Proccessor and Submit Task Again\n");
        return false;
    }
    bool startProcess()
    {
        printf("Task Proccessor is Completed, Clear Task Proccessor and Submit Task Again\n");
        return false;
    }
    bool clearAllTask()
    {
        return true;
    }
};

/****************************************************************************************/
// TaskProcessor

static unsigned int __stdcall executeOnThread(void* data)
{
    //printf("Thread %d\n", GetCurrentThreadId());
    if ( data == NULL ) {
        printf("Thread %d: Data is NULL\n", GetCurrentThreadId());
        return -1;
    }

    ITask *task = (ITask *)data;
    task->execute();
    return 0;
}

CTaskProcessor::CTaskProcessor(void)
{
    STATUS_NO_TASK          = new NoTaskState       (*this);
    STATUS_IS_IN_PROGRESS   = new IsInProgressState (*this);
    STATUS_READY            = new TaskReadyState    (*this);
    STATUS_COMPLETED        = new TaskCompletedState(*this);

    mStatus = STATUS_NO_TASK;
}

CTaskProcessor::~CTaskProcessor(void)
{
    // Make sure all task are completed before exit
    while (!mStatus->clearAllTask());

    mTaskList.clear();
    mStatus = STATUS_NO_TASK;

    delete STATUS_NO_TASK;
    delete STATUS_IS_IN_PROGRESS;
    delete STATUS_READY;
    delete STATUS_COMPLETED;
}

void CTaskProcessor::submitTask(ITask *task)
{
    if (!mStatus->submitTask()) return;

    mTaskList.push_back(task);

    mStatus = STATUS_READY;
}

void CTaskProcessor::startProcess()
{
    if (!mStatus->startProcess()) return;

    mStatus = STATUS_IS_IN_PROGRESS;

    const int taskSize = (int)mTaskList.size();

    if ( taskSize <= MAXIMUM_WAIT_OBJECTS )
    {
        for ( int i = 0; i < taskSize; i++)
        {
            HANDLE myhandle = (HANDLE)_beginthreadex(0, 0, &executeOnThread, mTaskList[i], CREATE_SUSPENDED, 0);
            mThreadHandle.push_back( myhandle );
        }

        for ( int i = 0; i < taskSize; i++)
            ResumeThread(mThreadHandle[i]);

        WaitForMultipleObjects(taskSize, &mThreadHandle[0], true, INFINITE);

        for ( int i = 0; i < (int)mThreadHandle.size(); i++) {
            CloseHandle(mThreadHandle[i]);
        }
    }
    else
    {
        // More MAXIMUM_WAIT_OBJECTS tasks
        /* If you need to wait on more than MAXIMUM_WAIT_OBJECTS handles, you can either create a separate thread to wait on MAXIMUM_WAIT_OBJECTS and
        then do a wait on these threads to finish. Using this method you can create MAXIMUM_WAIT_OBJECTS threads each of those can wait for MAXIMUM_WAIT_OBJECTS object handles.
        */
        const int groupSize = taskSize/MAXIMUM_WAIT_OBJECTS;

        for ( int i = 0; i < groupSize; i++ )
        {
            mThreadHandle.clear();
            for ( int task = 0; task < MAXIMUM_WAIT_OBJECTS; task++)
            {
                HANDLE myhandle = (HANDLE)_beginthreadex(0, 0, &executeOnThread, mTaskList[(i*MAXIMUM_WAIT_OBJECTS)+task], CREATE_SUSPENDED, 0);
                mThreadHandle.push_back( myhandle );
            }

            for ( int i = 0; i < MAXIMUM_WAIT_OBJECTS; i++)
                ResumeThread(mThreadHandle[i]);

            WaitForMultipleObjects(MAXIMUM_WAIT_OBJECTS, &mThreadHandle[0], true, INFINITE);

            for ( int i = 0; i < (int)mThreadHandle.size(); i++) {
                CloseHandle(mThreadHandle[i]);
            }
        }

        // Leftovers (if any)
        {
            const int tasksLeft = taskSize - (groupSize*MAXIMUM_WAIT_OBJECTS);
            if ( tasksLeft )
            {
                mThreadHandle.clear();
                for ( int task = groupSize*MAXIMUM_WAIT_OBJECTS; task < (int)mTaskList.size(); task++)
                {
                    HANDLE myhandle = (HANDLE)_beginthreadex(0, 0, &executeOnThread, mTaskList[task], CREATE_SUSPENDED, 0);
                    mThreadHandle.push_back( myhandle );
                }

                for ( int i = 0; i < (int)mThreadHandle.size(); i++)
                    ResumeThread(mThreadHandle[i]);

                WaitForMultipleObjects((int)mThreadHandle.size(), &mThreadHandle[0], true, INFINITE);

                for ( int i = 0; i < (int)mThreadHandle.size(); i++)
                {
                    CloseHandle(mThreadHandle[i]);
                }
            }
        }
    }

    mStatus = STATUS_COMPLETED;
}

void CTaskProcessor::clearAllTask()
{
    // Make sure all task are completed before clearing all task
    while (!mStatus->clearAllTask()) return;

    mTaskList.clear();
    mThreadHandle.clear();

    mStatus = STATUS_NO_TASK;
}

int CTaskProcessor::maxTaskCount()
{
    return MAXIMUM_WAIT_OBJECTS;
}