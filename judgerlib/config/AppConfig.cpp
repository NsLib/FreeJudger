#include "AppConfig.h"

#include "../logger/Logger.h"
#include "../xml/Xml.h"

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
    OJInt32_t   CompileError        =   11;
    OJInt32_t   CompileOK           =   12;
    OJInt32_t   CompileTimeError    =   14;
    OJInt32_t   SystemError         =   13;
    OJInt32_t   UnknownError        =   99;
}

namespace Language
{
    OJInt32_t   C                   = 0;
    OJInt32_t   Cxx                 = 1;
    OJInt32_t   Pascal              = 2;
    OJInt32_t   Java                = 3;
}

namespace Path
{
    OJString    TestDataPath(GetOJString("D:\\testdata"));
}

namespace WindowsUser
{
    bool        Enable      = true;
    OJString    Name        = OJStr("acmer");
    OJString    Password    = OJStr("imustacm");
}

bool InitAppConfig()
{
#define READ_APP_CONFIG(fun, tag, value)     \
    if(!root->fun(OJStr(tag), value))        \
    {   \
        logger->logError(OJStr("[config] - IMUST::AppConfig::InitAppConfig: read tag faild! ")##OJStr(tag));   \
        return false; \
    }

    ILogger *logger = LoggerFactory::getLogger(LoggerId::AppInitLoggerId);
    logger->logTrace(GetOJString("[config] - IMUST::AppConfig::InitAppConfig"));

    XmlPtr root = XmlFactory::getXml(GetOJString("RapidXml"));
    if(!root->load(OJStr("config.xml")))
    {
        logger->logError(OJStr("[config] - load config file faild! "));
        return false;
    }

    root = root->read(OJStr("AppConfig"));
    if(!root)
    {
        logger->logError(OJStr("[config] - read tag 'AppConfig' faild! "));
        return false;
    }

    READ_APP_CONFIG(readInt32, "CpuInfo/NumberOfCore", CpuInfo::NumberOfCore);

    READ_APP_CONFIG(readString, "MySql/Ip", MySql::Ip);
    READ_APP_CONFIG(readInt32,  "MySql/Port", MySql::Port);
    READ_APP_CONFIG(readString, "MySql/User", MySql::User);
    READ_APP_CONFIG(readString, "MySql/Password", MySql::Password);

    READ_APP_CONFIG(readBool, "Compiler/HasMsC", Compiler::HasMsC);
    READ_APP_CONFIG(readBool, "Compiler/HasMsCxx", Compiler::HasMsCxx);
    READ_APP_CONFIG(readBool, "Compiler/HasGcc", Compiler::HasGcc);
    READ_APP_CONFIG(readBool, "Compiler/HasGxx", Compiler::HasGxx);
    READ_APP_CONFIG(readBool, "Compiler/HasPascal", Compiler::HasPascal);
    READ_APP_CONFIG(readBool, "Compiler/HasJava", Compiler::HasJava);
    READ_APP_CONFIG(readBool, "Compiler/HasPython", Compiler::HasPython);

    READ_APP_CONFIG(readInt32, "JudgeCode/Pending", JudgeCode::Pending);
    READ_APP_CONFIG(readInt32, "JudgeCode/Rejudge", JudgeCode::Rejudge);
    READ_APP_CONFIG(readInt32, "JudgeCode/Compiling", JudgeCode::Compiling);
    READ_APP_CONFIG(readInt32, "JudgeCode/Accept", JudgeCode::Accept);
    READ_APP_CONFIG(readInt32, "JudgeCode/PresentError", JudgeCode::PresentError);
    READ_APP_CONFIG(readInt32, "JudgeCode/WrongAnswer", JudgeCode::WrongAnswer);
    READ_APP_CONFIG(readInt32, "JudgeCode/TimeLimitExceed", JudgeCode::TimeLimitExceed);
    READ_APP_CONFIG(readInt32, "JudgeCode/MemoryLimitExceed", JudgeCode::MemoryLimitExceed);
    READ_APP_CONFIG(readInt32, "JudgeCode/OutputLimited", JudgeCode::OutputLimited);
    READ_APP_CONFIG(readInt32, "JudgeCode/RuntimeError", JudgeCode::RuntimeError);
    READ_APP_CONFIG(readInt32, "JudgeCode/CompileError", JudgeCode::CompileError);
    READ_APP_CONFIG(readInt32, "JudgeCode/CompileTimeError", JudgeCode::CompileTimeError);
    READ_APP_CONFIG(readInt32, "JudgeCode/CompileOK", JudgeCode::CompileOK);
    READ_APP_CONFIG(readInt32, "JudgeCode/SystemError", JudgeCode::SystemError);
    READ_APP_CONFIG(readInt32, "JudgeCode/UnknownError", JudgeCode::UnknownError);

    READ_APP_CONFIG(readString, "Path/TestDataPath", Path::TestDataPath);

    READ_APP_CONFIG(readBool, "WindowsUser/Enable", WindowsUser::Enable);
    READ_APP_CONFIG(readString, "WindowsUser/Name", WindowsUser::Name);
    READ_APP_CONFIG(readString, "WindowsUser/Password", WindowsUser::Password);
   
#undef READ_APP_CONFIG

    return true;
}
}   // namespace AppConfig
}   // namespace IMUST
