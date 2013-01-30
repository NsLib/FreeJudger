#pragma once


#include "../platformlayer/PlatformLayer.h"
#include "../util/Utility.h"
#include "Sql.h"

#include <memory>

namespace IMUST
{

class TaskManager;
typedef std::shared_ptr<TaskManager> TaskManagerPtr;

class DBManager
{
    MAKE_CLASS_UNCOPYABLE(DBManager);

public:
    DBManager(SqlDriverPtr sqlDriver, TaskManagerPtr working, TaskManagerPtr finished);
    virtual ~DBManager(void);

    bool run();

private:

    bool readTask();

    bool writeFinishedTask();

private:
    SqlDriverPtr   sqlDriver_;
    TaskManagerPtr workingTaskMgr_;
    TaskManagerPtr finishedTaskMgr_;
};



}//namespace IMUST