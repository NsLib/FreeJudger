
#ifndef IMUST_OJ_TASK_MANAGER_H
#define	IMUST_OJ_TASK_MANAGER_H

#include <list>

#include "../util/Utility.h"
#include "../platformlayer/PlatformLayer.h"
#include "../thread/Thread.h"

namespace IMUST
{

struct JUDGER_API TaskInputData
{
    OJInt32_t   SolutionID;
    OJInt32_t   ProblemID;
    OJInt32_t   Language;
    OJInt32_t   LimitTime;
    OJInt32_t   LimitMemory;
    OJString    UserName;
    OJString    UserCode;
    OJString    UserInput;
};

struct JUDGER_API TaskOutputData
{
    OJInt32_t   Result;
    OJInt32_t   RunTime;
    OJInt32_t   RunMemory;
    OJFloat16_t PassRate;//测试数据通过的比例
    OJString    CompileError;
    OJString    RunTimeError;
};

class JUDGER_API ITask
{
public:
    ITask();
    virtual ~ITask();

    virtual bool run() = 0;

    virtual const TaskOutputData & output() const = 0;

    virtual const TaskInputData & input() const = 0;
};

class JUDGER_API TaskManager
{
public:
    TaskManager();
    ~TaskManager();

    void lock();
    void unlock();

    bool hasTask();
    void addTask(ITask *task);
    ITask *popTask();

private:
    typedef std::list<ITask *> TaskListType;
    TaskListType taskList_;

    Mutex   mutex_;
};


class TaskFactory
{
public:
    TaskFactory();
    virtual ~TaskFactory();

    virtual ITask* create(const TaskInputData & input) = 0;

    virtual void destroy(ITask* pTask) = 0;
};


typedef std::shared_ptr<TaskManager> TaskManagerPtr;
typedef std::shared_ptr<TaskFactory> TaskFactoryPtr;


}   // namespace IMUST

#endif  // IMUST_OJ_TASK_MANAGER_H