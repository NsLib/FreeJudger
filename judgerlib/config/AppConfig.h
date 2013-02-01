#ifndef IMUST_OJ_APP_CONFIG_H
#define IMUST_OJ_APP_CONFIG_H

#include "../platformlayer/PlatformLayer.h"

namespace IMUST
{
namespace AppConfig
{

bool JUDGER_API InitAppConfig();


namespace CpuInfo
{
    extern OJInt32_t   NumberOfCore;
}

namespace MySql
{
    extern OJString    Ip;
    extern OJInt32_t   Port;
    extern OJString    User;
    extern OJString    Password;
    extern OJString    DBName;
}

namespace Compiler
{
    extern bool        HasMsC;
    extern bool        HasMsCxx;
    extern bool        HasGcc;
    extern bool        HasGxx;
    extern bool        HasPascal;
    extern bool        HasJava;
    extern bool        HasPython;
}

namespace JudgeCode
{
    extern OJInt32_t   Pending;
    extern OJInt32_t   Rejudge;
    extern OJInt32_t   Compiling;
    extern OJInt32_t   Running;
    extern OJInt32_t   Accept;
    extern OJInt32_t   PresentError;
    extern OJInt32_t   WrongAnswer;
    extern OJInt32_t   TimeLimitExceed;
    extern OJInt32_t   MemoryLimitExceed;
    extern OJInt32_t   OutputLimited;
    extern OJInt32_t   RuntimeError;
    extern OJInt32_t   CompileTimeError;
    extern OJInt32_t   CompileOK;
    extern OJInt32_t   CompileError;
    extern OJInt32_t   SystemError;
    extern OJInt32_t   UnknownError;
}

namespace Path
{
    extern OJString    TestDataPath;
}


}   // AppConfig
}   // IMUST

#endif  // IMUST_OJ_APP_CONFIG_H
