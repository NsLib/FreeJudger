#include "Task.h"

#include "../judgerlib/taskmanager/TaskManager.h"
#include "../judgerlib/config/AppConfig.h"

extern bool g_sigExit;

namespace IMUST
{

JudgeTask::JudgeTask(const TaskInputData & inputData) 
    : Input(inputData)
{}

bool JudgeTask::run()
{
    static ILogger *logger = LoggerFactory::getLogger(LoggerId::AppInitLoggerId);
    OJChar_t buf[256];
    OJSprintf(buf, OJStr("task %d"), Input.SolutionID);
    OJString str(buf);
    logger->logInfo(str);


    if(!compile())
    {
        return false;
    }

    OJInt32_t testCount = 1;
    if(testCount <= 0)
    {
        output_.Result = AppConfig::JudgeCode::SystemError;

        OJSprintf(buf, OJStr("not found test data for solution %d problem %d."),
            Input.SolutionID, Input.ProblemID);
        logger->logError(buf);
        return false;
    }

    OJInt32_t accepted = 0;
    for(OJInt32_t i=0; i<testCount; ++i)
    {
        if(excute())
        {
            continue;
        }

        if(match())
        {
            ++accepted;
        }
    }

    output_.PassRate = float(accepted)/testCount;

    return true;
}

bool JudgeTask::compile()
{
    return false;
}

bool JudgeTask::excute()
{
    return false;
}

bool JudgeTask::match()
{
    return false;
}



JudgeThread::JudgeThread(int id, IMUST::TaskManagerPtr working, IMUST::TaskManagerPtr finish)
    : id_(id)
    , workingTaskMgr_(working)
    , finisheTaskMgr_(finish)
{

}

void JudgeThread::operator()()
{
    while (!g_sigExit)
    {
        static ILogger *logger = LoggerFactory::getLogger(LoggerId::AppInitLoggerId);

        IMUST::ITask* pTask = NULL;

        //从任务队列取任务
        workingTaskMgr_->lock();
        if(workingTaskMgr_->hasTask())
        {
            pTask = workingTaskMgr_->popTask();
        }
        workingTaskMgr_->unlock();

        if(!pTask)//没有任务
        {
            Sleep(1000);
            continue;
        }

        pTask->run();
            
        //添加到完成队列
        finisheTaskMgr_->lock();
        finisheTaskMgr_->addTask(pTask);
        finisheTaskMgr_->unlock();

        Sleep(10);//防止线程过度繁忙
    }

}

ITask* JudgeTaskFactory::create(const TaskInputData & input)
{
    return new JudgeTask(input);
}

void JudgeTaskFactory::destroy(ITask* pTask)
{
    delete pTask;
}


JudgeDBRunThread::JudgeDBRunThread(IMUST::DBManagerPtr dbm)
    : dbm_(dbm)
{
}

void JudgeDBRunThread::operator()()
{
    IMUST::ILogger *logger = IMUST::LoggerFactory::getLogger(IMUST::LoggerId::AppInitLoggerId);
    logger->logTrace(GetOJString("db thread start..."));

    while(!g_sigExit)
    {
        if(!dbm_->run())
        {
            logger->logError(GetOJString("db thread was dead!"));
            break;
        }
        Sleep(100);
    }

    logger->logTrace(GetOJString("db thread end."));
}

}   // namespace IMUST