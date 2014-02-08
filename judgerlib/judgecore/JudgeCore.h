#pragma once

namespace IMUST
{

typedef std::vector<ThreadPtr> JudgeThreadVector;
typedef std::shared_ptr<class WindowsUser> WindowsUserPtr;

class JudgeCore
{
public:
    JudgeCore();
    ~JudgeCore();

    bool startService();

    void stopService();

    bool isRunning() const { return running_; }

    TaskManagerPtr getWorkingTaskMgr(){ return workingTaskMgr_; }
    TaskManagerPtr getFinishedTaskMgr(){ return finishedTaskMgr_; }
    DBManagerPtr getDBManager(){ return dbManager_; }

private:
    bool                running_;
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