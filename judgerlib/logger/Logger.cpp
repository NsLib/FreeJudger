
#include "Logger.h"



namespace IMUST
{

LoggerFactory::SharedLoggerList LoggerFactory::loggers_(
    new LoggerFactory::LoggerList);

Logger &LoggerFactory::getLogger(const OJInt32_t loggerId)
{
    assert(loggerId >= 0 && "Invalid loggerId");
    assert(0 != (*loggers_)[loggerId] && "No such logger");
    return *((*loggers_)[loggerId]);
}

bool LoggerFactory::registerLogger(Logger *logger, const OJInt32_t loggerId)
{
    assert(loggerId >= 0 && "Invalid loggerId.");
    (*loggers_)[loggerId] = logger;
    return true;
}


Logger::Logger(ILogger* logger) :
    logger_(logger)
{
    assert(0 != logger && "logger can not be null");
}

Logger::~Logger()
{
    JUDGER_SAFE_DELETE_OBJ(logger_);
}

void Logger::logFatal(const OJString &msg)
{
    logger_->logFatal(msg);
}

void Logger::logError(const OJString &msg)
{
    logger_->logError(msg);
}

void Logger::logWarn(const OJString &msg)
{
    logger_->logWarn(msg);
}

void Logger::logInfo(const OJString &msg)
{
    logger_->logInfo(msg);
}

void Logger::logDebug(const OJString &msg)
{
    logger_->logDebug(msg);
}

void Logger::logTrace(const OJString &msg)
{
    logger_->logTrace(msg);
}

ILogger::ILogger()
{

}

ILogger::~ILogger()
{

}



}   // namespace IMUST
