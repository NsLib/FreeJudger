#include "AppConfig.h"

#include "../logger/Logger.h"

namespace IMUST
{
namespace AppConfig
{

namespace CpuInfo
{
    OJInt32_t   NumberOfCore        =   1;
}

namespace MySql
{
    OJString    Ip(GetOJString("127.0.0.1"));
    OJInt32_t   Port                =   3306;
    OJString    User(GetOJString("root"));
    OJString    Password(GetOJString(""));
    OJString    DBName(GetOJString("acmicpc"));
}

namespace Compiler
{
    bool        HasMsC              =   false;
    bool        HasMsCxx            =   false;
    bool        HasGcc              =   false;
    bool        HasGxx              =   false;
    bool        HasPascal           =   false;
    bool        HasJava             =   false;
    bool        HasPython           =   false;
}

namespace JudgeCode
{
    OJInt32_t   Pending             =   0;
    OJInt32_t   Rejudge             =   1;
    OJInt32_t   Compiling           =   2;
    OJInt32_t   Running             =   3;
    OJInt32_t   Accept              =   4;
    OJInt32_t   PresentError        =   5;
    OJInt32_t   WrongAnswer         =   6;
    OJInt32_t   TimeLimitExceed     =   7;
    OJInt32_t   MemoryLimitExceed   =   8;
    OJInt32_t   OutputLimited       =   9;
    OJInt32_t   RuntimeError        =   10;
    OJInt32_t   ComileTimeError     =   11;
    OJInt32_t   CompileOK           =   12;
    OJInt32_t   SystemError         =   13;
    OJInt32_t   UnknownError        =   99;
}

namespace Path
{
    OJString    TestDataPath(GetOJString("D:\\testdata"));
}

bool InitAppConfig()
{
    // TODO: ÒÀÀµxmlÏîÄ¿
    LoggerFactory::getLogger(LoggerId::AppInitLoggerId)->logInfo(
        GetOJString("config.IMUST::AppConfig::InitAppConfig"));



    return true;
}
}   // namespace AppConfig
}   // namespace IMUST
