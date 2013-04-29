#pragma once

#include "../judgerlib/taskmanager/TaskManager.h"
#include "../judgerlib/thread/Thread.h"
#include "../judgerlib/sql/DBManager.h"
#include "../judgerlib/process/WindowsUser.h"

namespace IMUST
{

typedef std::shared_ptr<IMUST::Thread> ThreadPtr;
typedef std::vector<ThreadPtr> JudgeThreadVector;

class JudgeCore
{
public:
    JudgeCore();
    ~JudgeCore();

    bool startService();

    void stopService();

private:
    SqlDriverPtr        mysql_;
    DBManagerPtr        dbManager_;
    TaskManagerPtr      workingTaskMgr_;
    TaskManagerPtr      finishedTaskMgr_;
    TaskFactoryPtr      taskFactory_;

    ThreadPtr           dbThread_;
    JudgeThreadVector   judgeThreadPool_;

    WindowsUserPtr       windowsUser_;
};

}//namespace IMUST