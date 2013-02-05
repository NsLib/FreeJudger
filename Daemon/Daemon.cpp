#include "Daemon.h"

#include <cstdlib>

#include "InitApp.h"
#include "../judgerlib/logger/Logger.h"
#include "../judgerlib/logger/Logger_log4cxx.h"
#include "../judgerlib/config/AppConfig.h"
#include "../judgerlib/taskmanager/TaskManager.h"
#include "../judgerlib/thread/Thread.h"
#include "../judgerlib/sql/DBManager.h"
#include "../judgerlib/filetool/FileTool.h"
#include "Task.h"

#include "../judgerlib/process/Process.h"

bool g_sigExit = false;


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

    if (!IMUST::InitApp())
        return EXIT_FAILURE;

    IMUST::ILogger *logger = IMUST::LoggerFactory::getLogger(IMUST::LoggerId::AppInitLoggerId);

    IMUST::SqlDriverPtr mysql = IMUST::SqlFactory::createDriver(IMUST::SqlType::MySql);
    if(!mysql->loadService())
    {
        IMUST::OJString msg(GetOJString("[Daemon] - WinMain - mysql->loadService failed - "));
        msg += mysql->getErrorString();
        logger->logError(msg);
        return EXIT_FAILURE;
    }

    if(!mysql->connect(IMUST::AppConfig::MySql::Ip, 
                    IMUST::AppConfig::MySql::Port,
                    IMUST::AppConfig::MySql::User,
                    IMUST::AppConfig::MySql::Password,
                    IMUST::AppConfig::MySql::DBName))
    {
        IMUST::OJString msg(GetOJString("[Daemon] - WinMain - connect mysql faild - "));
        msg += mysql->getErrorString();
        logger->logError(msg);
        return EXIT_FAILURE;
    }
    mysql->setCharSet(GetOJString("utf-8"));

    // 测试数据库管理器
    IMUST::TaskManagerPtr workingTaskMgr(new IMUST::TaskManager()); 
    IMUST::TaskManagerPtr finishedTaskMgr(new IMUST::TaskManager());
    IMUST::TaskFactoryPtr taskFactory(new IMUST::JudgeTaskFactory());

    IMUST::DBManagerPtr dbManager(new IMUST::DBManager(mysql, 
        workingTaskMgr, finishedTaskMgr, taskFactory));

    
    IMUST::FileTool::MakeDir(OJStr("work"));

    typedef std::shared_ptr<IMUST::Thread> ThreadPtr;
    std::vector<ThreadPtr> judgeThreadPool;
    for(IMUST::OJInt32_t i=0; i<IMUST::AppConfig::CpuInfo::NumberOfCore; ++i)
    {
        ThreadPtr ptr(new IMUST::Thread(IMUST::JudgeThread(i, workingTaskMgr, finishedTaskMgr)));
        judgeThreadPool.push_back(ptr);
    }
    
    IMUST::JudgeDBRunThread dbThreadObj(dbManager);
    IMUST::Thread dbThread(dbThreadObj);

    system("pause");

    g_sigExit = true;

    dbThread.join();

    for(std::vector<ThreadPtr>::iterator it = judgeThreadPool.begin();
        it != judgeThreadPool.end(); ++it)
    {
        (*it)->join();
    }

    mysql->disconect();
    mysql->unloadService();

    return EXIT_SUCCESS;
}
