
#define JUDGER_IMPORT

#include "../judgerlib/logger/Logger.h"
#include "../judgerlib/logger/Logger_log4cxx.h"

#include "../judgerlib/thread/Thread.h"
#include "../judgerlib/xml/Xml.h"

#include "../judgerlib/process/Process.h"

#include "../judgerlib/sql/Sql.h"

#include "../judgerlib/filetool/FileTool.h"

#include "../judgerlib/taskmanager/TaskManager.h"

#include "../judgerlib/sql/DBManager.h"

#include <vector>

using namespace std;

void ThreadFun()
{
    IMUST::ILogger *logger = new IMUST::Log4CxxLoggerImpl(GetOJString("log.cfg"), GetOJString("logger1"));

    // FIXED ME:logger中文乱码
    logger->logDebug(GetOJString("thread log"));
}

IMUST::ILogger *g_logger = new IMUST::Log4CxxLoggerImpl(GetOJString("log.cfg"), GetOJString("logger1"));

#include <cstdio>
namespace IMUST
{
class MockTask : public ITask
{
public:
    MockTask(int id) : id_(id) {}

    virtual bool run()
    {
        OJChar_t buf[20];
        wsprintf(buf, OJStr("task %d"), id_);
        OJString str(buf);
        g_logger->logInfo(str);
        return true;
    }

private:
    int id_;
};
}   // namespace IMUST

IMUST::TaskManager g_taskManager;

struct TaskThread
{
    TaskThread(int id) : id_(id) {}
    void operator()()
    {
        static int i = 10;
        static int j = 20;

        while (true)
        {
            g_taskManager.lock();
            IMUST::OJString str(OJStr("thread "));
            str += (id_ + '0');

            g_logger->logInfo(str);

            if (g_taskManager.hasTask())
            {
                g_taskManager.popTask()->run();
                g_taskManager.unlock();
                Sleep(50);
            } 
            else
            {
                if (i >= 100)
                {
                    g_taskManager.unlock();
                    break;
                }
                g_logger->logInfo(GetOJString("add task"));
                for (; i < j; ++i)
                    g_taskManager.addTask(new IMUST::MockTask(i));
                j += 10;
                g_taskManager.unlock();
                Sleep(50);
            }
        }

    }

    int id_;
};



int main()
{
// 测试日志
#if 0
    vector<IMUST::ILogger *> iloggers;
    vector<IMUST::ILogger *> loggers;

    iloggers.push_back(new IMUST::Log4CxxLoggerImpl(GetOJString("log.cfg"), GetOJString("logger1")));
    iloggers.push_back(new IMUST::Log4CxxLoggerImpl(GetOJString("log.cfg"), GetOJString("logger2")));
    iloggers.push_back(new IMUST::Log4CxxLoggerImpl(GetOJString("log.cfg"), GetOJString("logger3")));

    for (int i = 0; i < 3; ++i)
        IMUST::LoggerFactory::registerLogger(iloggers[i], i);

    for (int i = 0; i < 3; ++i)
        loggers.push_back(IMUST::LoggerFactory::getLogger(i));

    for (int i = 0; i < 3; ++i)
    {
        loggers[i]->logFatal(GetOJString("Msg4cxx"));
        loggers[i]->logError(GetOJString("Msg"));
        loggers[i]->logWarn(GetOJString("Msg"));
        loggers[i]->logInfo(GetOJString("Msg"));
        loggers[i]->logDebug(GetOJString("Msg"));
        loggers[i]->logTrace(GetOJString("Msg"));
    }
#endif

// 测试线程
#if 0
    IMUST::Thread t(&ThreadFun);
    t.join();
#endif

// 测试XML
#if 0
    IMUST::XmlPtr xmlRoot = IMUST::allocateRapidXml();
    if(!xmlRoot->load(GetOJString("config.xml")))
    {
        OJCout<<GetOJString("read config fail!")<<std::endl;
        return 0;
    }
   
    OJCout<<GetOJString("read config.xml")<<std::endl;

    //test read node
    IMUST::XmlPtr ptr = xmlRoot->read(GetOJString("AppConfig/MySql/Ip"));
    while(ptr)
    {
        OJCout<<ptr->tag()<<GetOJString(" = ")<<ptr->value()<<std::endl;
        ptr = ptr->getNextSibling();
    }
    
    //test read many
    IMUST::XmlPtrVector vector;
    if(xmlRoot->reads(GetOJString("AppConfig/JudgeCode/Pending"), vector))
    {
        for(IMUST::XmlPtrVector::iterator it=vector.begin(); it != vector.end(); ++it)
        {
            OJCout<<(*it)->tag()<<GetOJString(" = ")<<(*it)->value()<<std::endl;
        }
    }

    //test read data
    IMUST::OJInt32_t t = 0;
    IMUST::OJString tag = GetOJString("AppConfig/MySql/Port");
    if(xmlRoot->readInt32(tag, t))
    {
        OJCout<<tag<<GetOJString(" = ")<<t<<std::endl;
    }

    //test write
    xmlRoot = IMUST::allocateRapidXml();

    xmlRoot->writeInt32(GetOJString("config/size/x"), t+5);
    xmlRoot->writeFloat16(GetOJString("testTag/a"), 7.1f);
    xmlRoot->writeFloat32(GetOJString("testTag/b"), 8.777);

    //test save
    xmlRoot->save(GetOJString("testConfig.xml"));
#endif

// 测试进程
#if 0
    IMUST::WindowsProcess wp;
    wp.create(GetOJString("calc.exe"));
#endif

// 测试文件操作
#if 1
    IMUST::ILogger *logger = new IMUST::Log4CxxLoggerImpl(GetOJString("log.cfg"), GetOJString("logger1"));
    IMUST::OJString path(OJStr("D:\\a.txt"));
    bool res = IMUST::FileTool::IsFileExist(path);
    if (res)
    {
        logger->logDebug(GetOJString("file exist"));
        IMUST::FileTool::RemoveFile(path);
    }
    else
        logger->logDebug(GetOJString("file not exist"));

    IMUST::FileTool::MakeDir(GetOJString("D:\\dir"));
    logger->logDebug(IMUST::FileTool::GetFullFileName(path));
    logger->logDebug(IMUST::FileTool::GetFilePath(path));
    logger->logDebug(IMUST::FileTool::GetFileExt(GetOJString("D:\\a.ext1.ext2.txt")));
    logger->logDebug(IMUST::FileTool::GetFileName(GetOJString("D:\\a.ext1.ext2.txt")));

    IMUST::FileTool::FileNameList files;
    if (IMUST::FileTool::GetSpecificExtFiles(files, GetOJString("D:\\Code\\SmartCar"), GetOJString(".cs"), true))
    {
        for (IMUST::FileTool::FileNameList::iterator iter = files.begin();
            files.end() != iter; ++iter)
        {
            logger->logDebug(*iter);
        }
    } 
    else
    {
        logger->logDebug(GetOJString("No files"));
    }

    if (IMUST::FileTool::GetSpecificExtFiles(files, GetOJString("D:\\Code\\SmartCar"), GetOJString(".cs"), false))
    {
        for (IMUST::FileTool::FileNameList::iterator iter = files.begin();
            files.end() != iter; ++iter)
        {
            logger->logDebug(*iter);
        }
    } 
    else
    {
        logger->logDebug(GetOJString("No files"));
    }

    vector<IMUST::OJChar_t> buf;
    if (IMUST::FileTool::ReadFile(buf, GetOJString("D:\\Code\\a.txt")))
    {
        for (vector<IMUST::OJChar_t>::iterator iter = buf.begin();
            buf.end() != iter; ++iter)
        {
            OJCerr << *iter;
            IMUST::OJString str;
            str += *iter;
            logger->logDebug(str);
        }

        OJCerr << std::endl;
        if (IMUST::FileTool::WriteFile(buf, GetOJString("D:\\Code\\b.cpp")))
        {
            logger->logDebug(GetOJString("write file OK"));
        } 
        else
        {
            logger->logDebug(GetOJString("Can't write file"));
        }
    } 
    else
    {
        logger->logDebug(GetOJString("Can't read file"));
    }
#endif
    
// 测试数据库
#if 1
    IMUST::SqlDriverPtr mysql = IMUST::SqlFactory::createDriver(IMUST::SqlType::MySql);
    if(!mysql->loadService())
    {
        OJCout<<GetOJString("loadService faild!")<<mysql->getErrorString()<<std::endl;
        return 0;
    }

    if(!mysql->connect(GetOJString("127.0.0.1"), 3306, GetOJString("root"),
        GetOJString(""), GetOJString("acmicpc")))
    {
        OJCout<<GetOJString("connect faild!")<<mysql->getErrorString()<<std::endl;
        return 0;
    }
    mysql->setCharSet(GetOJString("utf-8"));

//测试数据读取
#if 0
    if(mysql->query(GetOJString("select solution_id, problem_id, user_id, time, memory from solution")))
    {
        IMUST::SqlResultPtr result = mysql->storeResult();
        if(result)
        {
            IMUST::OJUInt32_t cols = result->getNbCols();
            for(IMUST::OJUInt32_t i=0; i<cols; ++i)
            {
                OJCout<<result->getFieldName(i)<<GetOJString("\t");
            }
            OJCout<<endl;
            IMUST::SqlRowPtr row(NULL);
            while(row = result->fetchRow())
            {
                for(IMUST::OJUInt32_t i=0; i<cols; ++i)
                {
                    OJCout<<(*row)[i].getString()<<GetOJString("\t");
                }
                OJCout<<endl;
            }
        }
    }
#endif

    // 测试数据库管理器

    IMUST::TaskManagerPtr workingTaskMgr(new IMUST::TaskManager()); 
    IMUST::TaskManagerPtr finishedTaskMgr(new IMUST::TaskManager());

    IMUST::DBManager dbManager(mysql, workingTaskMgr, finishedTaskMgr);

    dbManager.run();

    mysql->disconect();

    mysql->unloadService();
#endif

// 测试任务管理器
#if 0
    // 运行5秒后终止调试
    g_taskManager.lock();
    for (int i = 0; i < 10; ++i)
        g_taskManager.addTask(new IMUST::MockTask(i));
    g_taskManager.unlock();

    IMUST::Thread thread1(TaskThread(1));
    IMUST::Thread thread2(TaskThread(2));
    IMUST::Thread thread3(TaskThread(3));
    thread1.join();
    thread2.join();
    thread3.join();
#endif

    system("pause");
    return 0;
}
