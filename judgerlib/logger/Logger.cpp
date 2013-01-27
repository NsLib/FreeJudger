
#include "Logger.h"



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



}   // namespace IMUST
