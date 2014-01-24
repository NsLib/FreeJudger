
#ifndef IMUST_OJ_TASK_H
#define IMUST_OJ_TASK_H


namespace IMUST
{

class JudgeTask : public ITask
{
public:
    JudgeTask(const TaskInputData & inputData);
    
    virtual void init(OJInt32_t judgeID);

    virtual bool run();

    virtual const TaskOutputData & output() const
    {
        return output_;
    }

    virtual const TaskInputData & input() const
    {
        return Input;
    }

private:

    void doRun();

    bool doClean();

    bool compile();

    bool excute();

    bool match();

public:
    const TaskInputData Input;

private:
    OJInt32_t       judgeID_;
    TaskOutputData  output_;
    OJString        codeFile_;
    OJString        exeFile_;
    OJString        compileFile_;
    OJString        userOutputFile_;
    OJString        answerInputFile_;
    OJString        answerOutputFile_;
};

class JudgeThread
{
public:
    JudgeThread(int id, IMUST::TaskManagerPtr working, IMUST::TaskManagerPtr finish);
    void operator()();

private:
    int id_;
    IMUST::TaskManagerPtr workingTaskMgr_;
    IMUST::TaskManagerPtr finisheTaskMgr_;
};


class JudgeTaskFactory : public TaskFactory
{
public:
    JudgeTaskFactory(){}
    virtual ~JudgeTaskFactory(){}

    virtual TaskPtr create(const TaskInputData & input);
};


class JudgeDBRunThread
{
public:

    JudgeDBRunThread(IMUST::DBManagerPtr dbm);

    void operator()();

private:
    IMUST::DBManagerPtr dbm_;
};


}   // namespace IMUST


#endif  // IMUST_OJ_TASK_H
