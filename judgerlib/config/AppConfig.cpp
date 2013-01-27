#include "AppConfig.h"

namespace IMUST
{

    namespace CpuInfo
    {
        OJInt32_t   NumberOfCore;
    }

    namespace MySql
    {
        OJString    Ip;
        OJInt32_t   Port;
        OJString    User;
        OJString    Password;
        OJString    DBName;
    }

    namespace Compiler
    {
        bool        HasMsC;
        bool        HasMsCxx;
        bool        HasGcc;
        bool        HasGxx;
        bool        HasPascal;
        bool        HasJava;
        bool        HasPython;
    }

    namespace JudgeCode
    {
        OJInt32_t   Pending;
        OJInt32_t   Rejudge;
        OJInt32_t   Compiling;
        OJInt32_t   Running;
        OJInt32_t   Accept;
        OJInt32_t   PresentError;
        OJInt32_t   WrongAnswer;
        OJInt32_t   TimeLimitExceed;
        OJInt32_t   MemoryLimitExceed;
        OJInt32_t   OutputLimited;
        OJInt32_t   RuntimeError;
        OJInt32_t   ComileTimeError;
        OJInt32_t   CompileOK;
        OJInt32_t   SystemError;
        OJInt32_t   UnknownError;
    }

    namespace Path
    {
        OJString    TestDataPath;
    }

bool InitAppConfig()
{
    // TODO: “¿¿µxmlœÓƒø




    return true;
}

}   // namespace IMUST
