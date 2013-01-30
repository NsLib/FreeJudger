#include "Task.h"

#include "../judgerlib/taskmanager/TaskManager.h"

extern bool g_sigExit;

namespace IMUST
{

JudgeThread::JudgeThread(TaskManager &taskManager, const int id) :
    taskManager_(taskManager),
    id_(id)
{

}

void JudgeThread::operator()()
{
    static int i = 10;
    static int j = 20;

    while (!g_sigExit)
    {
        static ILogger *logger = LoggerFactory::getLogger(LoggerId::AppInitLoggerId);

        taskManager_.lock();
        IMUST::OJString str(OJStr("thread "));
        str += (id_ + '0');

        logger->logInfo(str);

        if (taskManager_.hasTask())
        {
            taskManager_.unlock();
            taskManager_.popTask()->run();
        } 
        else
        {
            // Ìí¼ÓÈÎÎñ
            logger->logInfo(GetOJString("add task"));
            taskManager_.unlock();
        }
    }

}


}   // namespace IMUST