#include "TaskManager.h"

namespace IMUST
{

ITask::ITask()
{

}

ITask::~ITask()
{

}

TaskManager::TaskManager()
{

}

TaskManager::~TaskManager()
{

}

void TaskManager::lock()
{
    mutex_.lock();
}

void TaskManager::unlock()
{
    mutex_.unlock();
}

void TaskManager::addTask(ITask *task)
{
    assert(0 != task && "task can not be NULL");
    taskList_.push_back(task);
}

bool TaskManager::hasTask()
{
    return !taskList_.empty();
}

ITask * TaskManager::popTask()
{
    assert(!taskList_.empty() && "tasklist is empty");
    static ITask *res;
    res = taskList_.front();
    taskList_.pop_front();
    return res;
}





}   // namespace IMUST