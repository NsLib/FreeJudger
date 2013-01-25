
#define JUDGER_IMPORT

#include "../judgerlib/logger/Logger.h"
#include "../judgerlib/logger/Logger_log4cxx.h"

#include <vector>

using namespace std;

int main()
{
    vector<IMUST::ILogger *> iloggers;
    vector<IMUST::Logger *> loggers;

    iloggers.push_back(new IMUST::Log4CxxLoggerImpl(GetOJString("log.cfg"), GetOJString("logger1")));
    iloggers.push_back(new IMUST::Log4CxxLoggerImpl(GetOJString("log.cfg"), GetOJString("logger2")));
    iloggers.push_back(new IMUST::Log4CxxLoggerImpl(GetOJString("log.cfg"), GetOJString("logger3")));

    for (int i = 0; i < 3; ++i)
        IMUST::LoggerFactory::registerLogger(new IMUST::Logger(iloggers[i]), i);

    for (int i = 0; i < 3; ++i)
        loggers.push_back(&IMUST::LoggerFactory::getLogger(i));

    for (int i = 0; i < 3; ++i)
    {
        loggers[i]->logFatal(GetOJString("Msg"));
        loggers[i]->logError(GetOJString("Msg"));
        loggers[i]->logWarn(GetOJString("Msg"));
        loggers[i]->logInfo(GetOJString("Msg"));
        loggers[i]->logDebug(GetOJString("Msg"));
        loggers[i]->logTrace(GetOJString("Msg"));
    }

    return 0;
}
