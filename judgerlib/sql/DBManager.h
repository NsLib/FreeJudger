#pragma once


#include "../platformlayer/PlatformLayer.h"
#include "../util/Utility.h"
#include "../taskmanager/TaskManager.h"
#include "Sql.h"

#include <memory>

namespace IMUST
{

class DBManager
{
    MAKE_CLASS_UNCOPYABLE(DBManager);

public:
    DBManager(SqlDriverPtr sqlDriver, 
        TaskManagerPtr working, 
        TaskManagerPtr finished, 
        TaskFactoryPtr factory);

    virtual ~DBManager(void);

    bool run();

private:

    bool readTasks();

    bool readDB();

    OJInt32_t readTaskData(TaskInputData & taskData);

    bool writeFinishedTask();

    bool writeToDB(const ITask* pTask);

private:
    SqlDriverPtr    sqlDriver_;
    TaskManagerPtr  workingTaskMgr_;
    TaskManagerPtr  finishedTaskMgr_;
    TaskFactoryPtr  taskFactory_;
};



}//namespace IMUST