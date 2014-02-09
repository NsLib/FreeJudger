#include "stdafx.h"
#include "Task.h"


#include "../compiler/Compiler.h"
#include "../excuter/Excuter.h"
#include "../matcher/Matcher.h"
#include "../util/Watch.h"

#include "JudgeCore.h"

namespace IMUST
{

namespace
{
    OJInt32_t NumJudgeTask = 0;
}

/* 延迟删除文件。
当某子进程尚未完全退出时，他占用的文件再次被打开或删除，都会失败。故作延迟，等待一段时间。
如果文件始终无法删除，将表示该子进程无法退出，这将是一个致命错误，评判线程应当结束。 */
bool safeRemoveFile(const OJString & file, ILogger *pLogger)
{
    OJString infoBuffer;

    for(OJInt32_t i=0; i<10; ++i)//尝试删除10次
    {
        if(FileTool::RemoveFile(file))
        {
            return true;
        }
        OJSleep(1000);

        pLogger->logWarnX( OJStr("safeRemoveFile '%s' faild with %d times. code:%d"), 
            file.c_str(), i+1, GetLastError());
    }

    return false;
}

//获得文件扩展名
OJString getLanguageExt(OJInt32_t language)
{
    if(language == AppConfig::Language::C)
    {
        return OJStr("c");
    }
    else if(language == AppConfig::Language::Cxx)
    {
        return OJStr("cpp");
    }
    else if(language == AppConfig::Language::Java)
    {
        return OJStr("java");
    }

    return OJStr("unknown");
}

OJString getExcuterExt(OJInt32_t language)
{
    if(language == AppConfig::Language::C)
    {
        return OJStr("exe");
    }
    else if(language == AppConfig::Language::Cxx)
    {
        return OJStr("exe");
    }
    else if(language == AppConfig::Language::Java)
    {
        return OJStr("class");
    }

    return OJStr("unknown");
}

JudgeTask::JudgeTask(const TaskInputData & inputData) 
    : Input(inputData)
    , threadId_(0)
{
    output_.Result = AppConfig::JudgeCode::SystemError;
    output_.PassRate = 0.0f;
    output_.RunTime = 0;
    output_.RunMemory = 0;

    IMUST::WatchTool::LockRoot();
    ++NumJudgeTask;
    IMUST::WatchTool::Root()->watch(OJStr("core/numJudgeTask"), NumJudgeTask);
    IMUST::WatchTool::UnlockRoot();
}

JudgeTask::~JudgeTask()
{
    IMUST::WatchTool::LockRoot();
    --NumJudgeTask;
    IMUST::WatchTool::Root()->watch(OJStr("core/numJudgeTask"), NumJudgeTask);
    IMUST::WatchTool::UnlockRoot();
}

void JudgeTask::init(OJInt32_t threadId)
{
    threadId_ = threadId;

    OJString codeExt = getLanguageExt(Input.Language);
    OJString exeExt = getExcuterExt(Input.Language);

    FormatString(codeFile_, OJStr("work/%d/Main.%s"), threadId_, codeExt.c_str());
    FormatString(exeFile_, OJStr("work/%d/Main.%s"), threadId_, exeExt.c_str());
    FormatString(compileFile_, OJStr("work/%d/compile.txt"), threadId_);
    FormatString(userOutputFile_, OJStr("work/%d/output.txt"), threadId_);

    FileTool::WriteFile(Input.UserCode, codeFile_);
}

bool JudgeTask::run()
{
    doRun();

    if(!doClean())
    {
        return false;//致命错误
    }

    return true;
}

void JudgeTask::doRun()
{
    ILogger *logger = LoggerFactory::getJudgeThreadLogger(threadId_);

    OJString infoBuffer;

    FormatString(infoBuffer, OJStr("[JudgeTask] task %d"), Input.SolutionID);
    logger->logInfo(infoBuffer);

    //编译
    if(!compile())
    {
        return;
    }

    //搜索测试数据

    OJString path;
    FormatString(path, OJStr("%s/%d"), AppConfig::Path::TestDataPath.c_str(), Input.ProblemID);
    
    DebugMessage(OJStr("[JudgeTask] %d search path: %s"), Input.SolutionID, path.c_str());

    //TODO: 根据是否specialJudge，决定搜索.out还是.in文件。
    FileTool::FileNameList fileList;
    FileTool::GetSpecificExtFiles(fileList, path, OJStr(".out"), true);

    OJUInt32_t testCount = fileList.size();
    if(testCount <= 0)//没有测试数据
    {
        output_.Result = AppConfig::JudgeCode::SystemError;

        FormatString(infoBuffer, OJStr("[JudgeTask] not found test data for solution %d problem %d."),
            Input.SolutionID, Input.ProblemID);
        logger->logError(infoBuffer);
        return;
    }

    //测试多组数据
    OJUInt32_t accepted = 0;
    for(OJUInt32_t i=0; i<testCount; ++i)
    {
        answerOutputFile_ = fileList[i];
        answerInputFile_ = FileTool::RemoveFileExt(answerOutputFile_);
        answerInputFile_ += OJStr(".in");

        DebugMessage(OJStr("[JudgeTask] %d input file: %s"), 
            Input.SolutionID, answerInputFile_.c_str());
        DebugMessage(OJStr("[JudgeTask] %d output file: %s"), 
            Input.SolutionID, answerOutputFile_.c_str());

        if(!safeRemoveFile(userOutputFile_, logger))
        {
            output_.Result = AppConfig::JudgeCode::SystemError;
            break;
        }

        if(!excute())
        {
            break;
        }
            
        if(!match())
        {
            break;
        }
        
        ++accepted;
    }

    output_.PassRate = float(accepted)/testCount;
}

bool JudgeTask::doClean()
{
    bool faild = false;

    ILogger *pLogger = LoggerFactory::getJudgeThreadLogger(threadId_);

    if(!safeRemoveFile(codeFile_, pLogger))
    {
        faild = true;
    }
    if(!safeRemoveFile(exeFile_, pLogger))
    {
        faild = true;
    }
    if(!safeRemoveFile(userOutputFile_, pLogger))
    {
        faild = true;
    }
    if(!safeRemoveFile(compileFile_, pLogger))
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
    ILogger *logger = LoggerFactory::getJudgeThreadLogger(threadId_);
    logger->logTrace(OJStr("[JudgeTask] start compile..."));
    
    CompilerPtr compiler = CompilerFactory::create(Input.Language);
    compiler->run(codeFile_, exeFile_, compileFile_);
    output_.Result = compiler->getResult();

    if(output_.Result == AppConfig::JudgeCode::CompileError)
    {
        output_.Result = AppConfig::JudgeCode::CompileError;
        
        std::vector<OJChar_t> buffer;
        if(FileTool::ReadFile(buffer, compileFile_) && !buffer.empty())
        {
            output_.CompileError = &buffer[0];
        }
    }

    return output_.Result == AppConfig::JudgeCode::Accept;
}

bool JudgeTask::excute()
{
    ILogger *logger = LoggerFactory::getJudgeThreadLogger(threadId_);
    logger->logTrace(OJStr("[JudgeTask] start excute..."));
    
    OJString infoBuffer;

    if(!FileTool::IsFileExist(exeFile_))
    {
        FormatString(infoBuffer, OJStr("[JudgeTask] not found exe file! %s."), exeFile_);
        logger->logError(infoBuffer);
        output_.Result = AppConfig::JudgeCode::SystemError;
        return false;
    }

    ExcuterPtr excuter = ExcuterFactory::create(Input.Language);
    excuter->run(exeFile_, answerInputFile_, userOutputFile_, Input.LimitTime, Input.LimitMemory);
    output_.Result = excuter->getResult();

    output_.RunTime = excuter->getRunTime();
    output_.RunMemory = excuter->getRunMemory();

    return output_.Result == AppConfig::JudgeCode::Accept;
}

bool JudgeTask::match()
{
    ILogger *logger = LoggerFactory::getJudgeThreadLogger(threadId_);
    logger->logTrace(OJStr("[JudgeTask] start match..."));

    MatcherPtr matcher = MatcherFactory::create(false, OJStr(""));
    matcher->run(answerOutputFile_, userOutputFile_);
    output_.Result = matcher->getResult();

    return output_.Result == AppConfig::JudgeCode::Accept;
}



JudgeThread::JudgeThread(int id, JudgeCore * pJudgeCore)
    : id_(id)
    , pJudgeCore_(pJudgeCore)
{

}

void JudgeThread::operator()()
{
    ILogger *logger = LoggerFactory::getJudgeThreadLogger(id_);

    OJString infoBuffer;
    FormatString(infoBuffer, OJStr("work/%d"), id_);
    FileTool::MakeDir(infoBuffer);

    logger->logTraceX(OJStr("[JudgeThread][%d]start..."), id_);

    static OJInt32_t s_numThread = 0;

    IMUST::WatchTool::LockRoot();
    ++s_numThread;
    IMUST::WatchTool::Root()->watch(OJStr("core/numThread"), s_numThread);
    IMUST::WatchTool::UnlockRoot();

    TaskManagerPtr workingTaskMgr = pJudgeCore_->getWorkingTaskMgr();
    TaskManagerPtr finishedTaskMgr = pJudgeCore_->getFinishedTaskMgr();

    while (pJudgeCore_->isRunning())
    {

        IMUST::TaskPtr pTask;

        //从任务队列取任务
        workingTaskMgr->lock();
        if(workingTaskMgr->hasTask())
        {
            pTask = workingTaskMgr->popTask();
        }
        workingTaskMgr->unlock();

        if(!pTask)//没有任务
        {
            OJSleep(1000);
            continue;
        }

        pTask->init(id_);
        if(!pTask->run())
        {
            logger->logErrorX(OJStr("[JudgeThread][%d]System Error!Judge thread will exit!"), id_);
            break;
        }

        //添加到完成队列
        finishedTaskMgr->lock();
        finishedTaskMgr->addTask(pTask);
        finishedTaskMgr->unlock();

        OJSleep(10);//防止线程过度繁忙
    }

    logger->logTraceX(OJStr("[JudgeThread][%d]end."), id_);

    IMUST::WatchTool::LockRoot();
    --s_numThread;
    IMUST::WatchTool::Root()->watch(OJStr("core/numThread"), s_numThread);
    IMUST::WatchTool::UnlockRoot();
}

TaskPtr JudgeTaskFactory::create(const TaskInputData & input)
{
    return TaskPtr(new JudgeTask(input));
}


JudgeDBRunThread::JudgeDBRunThread(JudgeCore * pJudgeCore)
    : pJudgeCore_(pJudgeCore)
{
}

void JudgeDBRunThread::operator()()
{
    IMUST::ILogger *logger = IMUST::LoggerFactory::getLogger(IMUST::LoggerId::AppInitLoggerId);
    logger->logTrace(GetOJString("[DBThread] thread start..."));

    DBManagerPtr dbm = pJudgeCore_->getDBManager();

    while(pJudgeCore_->isRunning())
    {
        if(!dbm->run())
        {
            logger->logError(GetOJString("[DBThread] db manager was dead!"));
            break;
        }
        OJSleep(100);
    }

    logger->logTrace(GetOJString("[DBThread] thread end."));
}

}   // namespace IMUST
