
#include "Logger.h"
#include "../util/StringTool.h"


namespace IMUST
{

LoggerFactory::SharedLoggerList LoggerFactory::loggers_(
    new LoggerFactory::LoggerList, LoggerFactory::deleter());

ILogger* LoggerFactory::getLogger(const OJInt32_t loggerId)
{
    assert(loggerId >= 0 && "Invalid loggerId");
    assert(0 != (*loggers_)[loggerId] && "No such logger");
    return (*loggers_)[loggerId];
}

bool LoggerFactory::registerLogger(ILogger *logger, const OJInt32_t loggerId)
{
    assert(loggerId >= 0 && "Invalid loggerId.");
    (*loggers_)[loggerId] = logger;
    return true;
}

ILogger::ILogger()
{

}

ILogger::~ILogger()
{

}

void ILogger::logFatalX(const OJChar_t * pFormat, ...)
{
    OJString strDest;

    va_list pArgList;
    va_start (pArgList, pFormat);
    FormatStringVS(strDest, pFormat, pArgList);
    va_end(pArgList);

    logFatal(strDest);
}

void ILogger::logErrorX(const OJChar_t * pFormat, ...)
{
    OJString strDest;

    va_list pArgList;
    va_start (pArgList, pFormat);
    FormatStringVS(strDest, pFormat, pArgList);
    va_end(pArgList);

    logError(strDest);
}

void ILogger::logWarnX(const OJChar_t * pFormat, ...)
{
    OJString strDest;

    va_list pArgList;
    va_start (pArgList, pFormat);
    FormatStringVS(strDest, pFormat, pArgList);
    va_end(pArgList);

    logWarn(strDest);
}

void ILogger::logInfoX(const OJChar_t * pFormat, ...)
{
    OJString strDest;

    va_list pArgList;
    va_start (pArgList, pFormat);
    FormatStringVS(strDest, pFormat, pArgList);
    va_end(pArgList);

    logInfo(strDest);
}

void ILogger::logDebugX(const OJChar_t * pFormat, ...)
{
    OJString strDest;

    va_list pArgList;
    va_start (pArgList, pFormat);
    FormatStringVS(strDest, pFormat, pArgList);
    va_end(pArgList);

    logDebug(strDest);
}

void ILogger::logTraceX(const OJChar_t * pFormat, ...)
{
    OJString strDest;

    va_list pArgList;
    va_start (pArgList, pFormat);
    FormatStringVS(strDest, pFormat, pArgList);
    va_end(pArgList);

    logTrace(strDest);
}

}   // namespace IMUST
