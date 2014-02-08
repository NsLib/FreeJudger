
#ifndef IMUST_OJ_TASK_H
#define IMUST_OJ_TASK_H


namespace IMUST
{

class JudgeCore;

class JudgeTask : public ITask
{
public:
    JudgeTask(const TaskInputData & inputData);
    
    virtual void init(OJInt32_t threadId);

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
    OJInt32_t       threadId_;
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
    JudgeThread(int id, JudgeCore * pJudgeCore);
    void operator()();

private:
    int id_;
    JudgeCore *pJudgeCore_;
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

    JudgeDBRunThread(JudgeCore * pJudgeCore);

    void operator()();

private:
    JudgeCore * pJudgeCore_;
};


}   // namespace IMUST


#endif  // IMUST_OJ_TASK_H
