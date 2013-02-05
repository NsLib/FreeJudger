#include "Task.h"

#include "../judgerlib/taskmanager/TaskManager.h"
#include "../judgerlib/config/AppConfig.h"
#include "../judgerlib/filetool/FileTool.h"


#include "Compiler.h"

extern bool g_sigExit;

#pragma warning(push)
#pragma warning(disable:4996)

namespace IMUST
{
bool safeRemoveFile(const OJString & file)
{
    OJChar_t buffer[128];

    for(OJInt32_t i=0; i<10; ++i)
    {
        if(FileTool::RemoveFile(file))
        {
            return true;
        }
        Sleep(1000);

        OJSprintf(buffer, OJStr("safeRemoveFile '%s' faild with %d times. code:%d"), 
            file.c_str(), i+1, GetLastError());
        LoggerFactory::getLogger(LoggerId::AppInitLoggerId)->logError(buffer);
    }

    return false;
}

OJString getLanguageExt(OJInt32_t language)
{
    if(language == 0)
    {
        return OJStr("c");
    }
    else if(language == 1)
    {
        return OJStr("cpp");
    }
    else if(language == 2)
    {
        return OJStr("java");
    }

    return OJStr("unknown");
}

JudgeTask::JudgeTask(const TaskInputData & inputData) 
    : Input(inputData)
    , judgeID_(0)
{
    output_.Result = AppConfig::JudgeCode::SystemError;
    output_.PassRate = 0.0f;
    output_.RunTime = 0;
    output_.RunMemory = 0;
}

void JudgeTask::init(OJInt32_t judgeID)
{
    judgeID_ = judgeID;

    OJString fileExt = getLanguageExt(Input.Language);

    OJChar_t buffer[1024];

    OJSprintf(buffer, OJStr("work\\%d\\Main.%s"), judgeID_, fileExt.c_str());
    codeFile_ = buffer;
    
    FileTool::WriteFile(Input.UserCode, codeFile_);

    OJSprintf(buffer, OJStr("work\\%d\\Main.exe"), judgeID_);
    exeFile_ = buffer;

    OJSprintf(buffer, OJStr("work\\%d\\compile.txt"), judgeID_);
    compileFile_ = buffer;
}

bool JudgeTask::run()
{
    doRun();

    if(!doClean())
    {
        return false;
    }

    return true;
}

bool JudgeTask::doRun()
{
    static ILogger *logger = LoggerFactory::getLogger(LoggerId::AppInitLoggerId);
    OJChar_t buf[256];
    OJSprintf(buf, OJStr("task %d"), Input.SolutionID);
    OJString str(buf);
    logger->logInfo(str);

    if(!compile())
    {
        return false;
    }

    //TODO: 搜索测试数据


    OJInt32_t testCount = 1;
    if(testCount <= 0)
    {
        output_.Result = AppConfig::JudgeCode::SystemError;

        OJSprintf(buf, OJStr("not found test data for solution %d problem %d."),
            Input.SolutionID, Input.ProblemID);
        logger->logError(buf);
        return false;
    }

    OJInt32_t accepted = 0;
    for(OJInt32_t i=0; i<testCount; ++i)
    {
        if(!excute())
        {
            continue;
        }

        if(match())
        {
            ++accepted;
        }
    }

    output_.PassRate = float(accepted)/testCount;
    return true;
}

bool JudgeTask::doClean()
{
    bool faild = false;

    if(!safeRemoveFile(codeFile_))
    {
        faild = true;
    }
    if(!safeRemoveFile(exeFile_))
    {
        faild = true;
    }
    if(!safeRemoveFile(compileFile_))
    {
        faild = true;
    }

    if(faild)
    {
        output_.Result = AppConfig::JudgeCode::SystemError;
    }

    return !faild;
}

bool JudgeTask::compile()
{
    static ILogger *logger = LoggerFactory::getLogger(LoggerId::AppInitLoggerId);
    CompilerPtr compiler = CompilerFactory::create(Input.Language);
    OJInt32_t code = compiler->run(codeFile_, exeFile_, compileFile_);
    if(code != 0)
    {
        output_.Result = AppConfig::JudgeCode::CompileError;
        
        //TODO: 读取编译错误信息
        
    }

    return code == 0;
}

bool JudgeTask::excute()
{
    return false;
}

bool JudgeTask::match()
{
    return false;
}



JudgeThread::JudgeThread(int id, IMUST::TaskManagerPtr working, IMUST::TaskManagerPtr finish)
    : id_(id)
    , workingTaskMgr_(working)
    , finisheTaskMgr_(finish)
{

}

void JudgeThread::operator()()
{
    ILogger *logger = LoggerFactory::getLogger(LoggerId::AppInitLoggerId);

    OJChar_t buffer[128];
    OJSprintf(buffer, OJStr("work/%d"), id_);
    FileTool::MakeDir(buffer);

    OJSprintf(buffer, OJStr("[JudgeThread][%d]start..."), id_);
    logger->logTrace(buffer);

    while (!g_sigExit)
    {

        IMUST::ITask* pTask = NULL;

        //从任务队列取任务
        workingTaskMgr_->lock();
        if(workingTaskMgr_->hasTask())
        {
            pTask = workingTaskMgr_->popTask();
        }
        workingTaskMgr_->unlock();

        if(!pTask)//没有任务
        {
            Sleep(1000);
            continue;
        }

        pTask->init(id_);
        if(!pTask->run())
        {
            OJSprintf(buffer, OJStr("[JudgeThread][%d]System Error!Judge thread will exit!"), id_);
            logger->logError(buffer);
            break;
        }

        //添加到完成队列
        finisheTaskMgr_->lock();
        finisheTaskMgr_->addTask(pTask);
        finisheTaskMgr_->unlock();

        Sleep(10);//防止线程过度繁忙
    }

    OJSprintf(buffer, OJStr("[JudgeThread][%d]end."), id_);
    logger->logTrace(buffer);

}

ITask* JudgeTaskFactory::create(const TaskInputData & input)
{
    return new JudgeTask(input);
}

void JudgeTaskFactory::destroy(ITask* pTask)
{
    delete pTask;
}


JudgeDBRunThread::JudgeDBRunThread(IMUST::DBManagerPtr dbm)
    : dbm_(dbm)
{
}

void JudgeDBRunThread::operator()()
{
    IMUST::ILogger *logger = IMUST::LoggerFactory::getLogger(IMUST::LoggerId::AppInitLoggerId);
    logger->logTrace(GetOJString("db thread start..."));

    while(!g_sigExit)
    {
        if(!dbm_->run())
        {
            logger->logError(GetOJString("db thread was dead!"));
            break;
        }
        Sleep(100);
    }

    logger->logTrace(GetOJString("db thread end."));
}

}   // namespace IMUST

#pragma warning(pop)