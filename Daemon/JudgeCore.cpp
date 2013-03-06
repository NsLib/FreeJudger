
#include "JudgeCore.h"

#include "../judgerlib/logger/Logger.h"
#include "../judgerlib/logger/Logger_log4cxx.h"
#include "../judgerlib/config/AppConfig.h"
#include "../judgerlib/filetool/FileTool.h"
#include "../judgerlib/process/Process.h"
#include "Task.h"

bool g_sigExit = false;

namespace IMUST
{

JudgeCore::JudgeCore()
    : mysql_(0)
    , dbManager_(0)
    , dbThread_(0)
    , workingTaskMgr_(new TaskManager())
    , finishedTaskMgr_(new TaskManager())
    , taskFactory_(new JudgeTaskFactory())
{
}

JudgeCore::~JudgeCore()
{
}

bool JudgeCore::startService()
{
    ILogger *logger = LoggerFactory::getLogger(LoggerId::AppInitLoggerId);

    mysql_ = SqlFactory::createDriver(SqlType::MySql);
    if(!mysql_->loadService())
    {
        OJString msg(OJStr("[Daemon] - WinMain - mysql->loadService failed - "));
        msg += mysql_->getErrorString();
        logger->logError(msg);
        return false;
    }

    if(!mysql_->connect(AppConfig::MySql::Ip, 
                    AppConfig::MySql::Port,
                    AppConfig::MySql::User,
                    AppConfig::MySql::Password,
                    AppConfig::MySql::DBName))
    {
        OJString msg(OJStr("[Daemon] - WinMain - connect mysql faild - "));
        msg += mysql_->getErrorString();
        logger->logError(msg);
        return false;
    }
    mysql_->setCharSet(OJStr("utf-8"));
    
    dbManager_ = DBManagerPtr(new DBManager(mysql_, 
        workingTaskMgr_, finishedTaskMgr_, taskFactory_));

    //TODO: 移除此处的测试操作
    dbManager_->doTestBeforeRun();

    //hook操作
    if(false && NULL == LoadLibraryW(L"windowsapihook.dll"))
    {
        logger->logWarn(GetOJString("[Daemon] - WinMain - load hook dll faild! - "));
    }

    //启动数据库线程
    dbThread_ = ThreadPtr(new Thread(JudgeDBRunThread(dbManager_)));


    FileTool::MakeDir(OJStr("work"));

    //启动评判线程
    for(IMUST::OJInt32_t i=0; i<AppConfig::CpuInfo::NumberOfCore; ++i)
    {
        ThreadPtr ptr(new IMUST::Thread(IMUST::JudgeThread(i, workingTaskMgr_, finishedTaskMgr_)));
        judgeThreadPool_.push_back(ptr);
    }
    
    return true;
}

void JudgeCore::stopService()
{
    g_sigExit = true;

    dbThread_->join();

    for(JudgeThreadVector::iterator it = judgeThreadPool_.begin();
        it != judgeThreadPool_.end(); ++it)
    {
        (*it)->join();
    }

    mysql_->disconect();
    mysql_->unloadService();

}

}//namespace IMUST