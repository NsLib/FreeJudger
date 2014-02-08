#include "stdafx.h"
#include "JudgeCore.h"

#include "../logger/Logger_log4cxx.h"
#include "../process/Process.h"
#include "../process/WindowsUser.h"

#include "Task.h"
#include "InitApp.h"


namespace IMUST
{

JudgeCore::JudgeCore()
    : running_(false)
    , mysql_(0)
    , dbManager_(0)
    , dbThread_(0)
    , workingTaskMgr_(new TaskManager())
    , finishedTaskMgr_(new TaskManager())
    , taskFactory_(new JudgeTaskFactory())
{
}

JudgeCore::~JudgeCore()
{
    if(running_)
    {
        DebugMessage(L"Warn: The judge core is already running!");
        stopService();
    }
}

bool JudgeCore::startService()
{
    if(running_)
    {
        LoggerFactory::getLogger(LoggerId::AppInitLoggerId)->logError(OJStr("JudgeCore is already running!"));
        return false;
    }

    if (!IsAppValid())
    {
        MessageBoxW(NULL, L"启动JudgeCore之前，必须先初始化App！", L"ERROR", MB_ICONSTOP);
        return false;
    }

    //创建工作目录
    if(!FileTool::MakeDir(OJStr("work")))
    {
        return false;
    }

    ILogger *logger = LoggerFactory::getLogger(LoggerId::AppInitLoggerId);
    logger->logTrace(OJStr("[JudgeCore] - startService"));

    //登录Windows判题用户
    if(AppConfig::WindowsUser::Enable)
    {
        windowsUser_ = WindowsUserPtr(new WindowsUser());
        if(!windowsUser_->login(
            AppConfig::WindowsUser::Name, 
            OJStr(""), 
            AppConfig::WindowsUser::Password))
        {
            logger->logError(OJStr("JudgeCore::startService login windows user failed. ")
                OJStr("You need to create an Windows user, or disable Windows user by modify the config.xml"));
            return false;
        }

        logger->logInfo(OJStr("login windows succed."));
        ProcessFactory::setWindowsUser(windowsUser_);
    }

    //创建sql设备
    mysql_ = SqlFactory::createDriver(SqlType::MySql);
    if(!mysql_->loadService())
    {
        OJString msg(OJStr("[JudgeCore] - WinMain - mysql->loadService failed - "));
        msg += mysql_->getErrorString();
        logger->logError(msg);
        return false;
    }

    //连接数据库
    if(!mysql_->connect(AppConfig::MySql::Ip, 
                    AppConfig::MySql::Port,
                    AppConfig::MySql::User,
                    AppConfig::MySql::Password,
                    AppConfig::MySql::DBName))
    {
        OJString msg(OJStr("[JudgeCore] - WinMain - connect mysql faild - "));
        msg += mysql_->getErrorString();
        logger->logError(msg);
        return false;
    }
    mysql_->setCharSet(OJStr("utf-8"));
    
    //创建数据库管理器
    dbManager_ = DBManagerPtr(new DBManager(mysql_, 
        workingTaskMgr_, finishedTaskMgr_, taskFactory_));

    //TODO: 移除此处的测试操作
    logger->logInfo(OJStr("clear db data."));
    dbManager_->doTestBeforeRun();

    running_ = true;

    //启动数据库线程
    dbThread_ = ThreadPtr(new Thread(JudgeDBRunThread(this)));

    //启动评判线程
    for(IMUST::OJInt32_t i=0; i<AppConfig::CpuInfo::NumberOfCore; ++i)
    {
        ThreadPtr ptr(new IMUST::Thread(IMUST::JudgeThread(i, this)));
        judgeThreadPool_.push_back(ptr);
    }
    
    return true;
}

void JudgeCore::stopService()
{
    if(!running_) return;

    running_ = false;

    ILogger *logger = LoggerFactory::getLogger(LoggerId::AppInitLoggerId);
    logger->logTrace(OJStr("[JudgeCore] - stopService"));

    for(JudgeThreadVector::iterator it = judgeThreadPool_.begin();
        it != judgeThreadPool_.end(); ++it)
    {
        if (*it)
        {
            (*it)->join();
        }
    }

    if (dbThread_)
    {
        dbThread_->join();
    }

    if (mysql_)
    {
        mysql_->disconect();
        mysql_->unloadService();
    }

    if (windowsUser_)
    {
        windowsUser_->logout();
    }
}

}//namespace IMUST