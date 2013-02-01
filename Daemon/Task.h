
#ifndef IMUST_OJ_TASK_H
#define IMUST_OJ_TASK_H

#include "../judgerlib/logger/Logger.h"
#include "../judgerlib/taskmanager/TaskManager.h"

namespace IMUST
{

class JudgeTask : public ITask
{
public:
    JudgeTask(int id) : id_(id) {}

    virtual bool run()
    {
        static ILogger *logger = LoggerFactory::getLogger(LoggerId::AppInitLoggerId);
        OJChar_t buf[20];
        wsprintf(buf, OJStr("task %d"), id_);
        OJString str(buf);
        logger->logInfo(str);
        return true;
    }

    virtual const TaskOutputData & output() const
    {
        return output_;
    }

    virtual const TaskInputData & input() const
    {
        return input_;
    }

private:
    int id_;
    TaskInputData input_;
    TaskOutputData output_;
};

class JudgeThread
{
public:
    JudgeThread(TaskManager &taskManager, const int id);
    void operator()();

private:
    int id_;
    TaskManager &taskManager_;
};



}   // namespace IMUST


#endif  // IMUST_OJ_TASK_H
