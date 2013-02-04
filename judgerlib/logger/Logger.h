
/**
 * @file    Logger.h
 * @brief   日志组件.
 */

#ifndef IMUST_OJ_LOGGER_H
#define	IMUST_OJ_LOGGER_H

#include <memory>
#include <map>

#include "../platformlayer/PlatformLayer.h"

namespace IMUST
{

class JUDGER_API ILogger
{
public:
    ILogger();
    virtual ~ILogger();

    virtual void logFatal(const OJString &msg) const = 0;
    virtual void logError(const OJString &msg) const = 0;
    virtual void logWarn(const OJString &msg) const = 0;
    virtual void logInfo(const OJString &msg) const = 0;
    virtual void logDebug(const OJString &msg) const = 0;
    virtual void logTrace(const OJString &msg) const = 0;
};

namespace LoggerId
{
const OJInt32_t AppInitLoggerId             = 0; 


}


namespace LogLevel
{
typedef OJInt32_t LogLevelType;

const LogLevelType Off   = 0;
const LogLevelType Fatal = 10;
const LogLevelType Error = 20;
const LogLevelType Warn  = 30;
const LogLevelType Info  = 40;
const LogLevelType Debug = 50;
const LogLevelType Trace = 60;
const LogLevelType All   = 70;
}   // namespace LogLevel

class JUDGER_API LoggerFactory
{
public:
    typedef std::map<OJInt32_t, ILogger*>               LoggerList;
    typedef std::shared_ptr<LoggerList>                 SharedLoggerList;

public:
    static ILogger* getLogger(const OJInt32_t loggerId);
    static bool registerLogger(ILogger *logger, const OJInt32_t loggerId);

private:
    LoggerFactory();
    ~LoggerFactory();

private:
    static SharedLoggerList loggers_;

private:
    struct deleter
    {
        void operator()(LoggerFactory::LoggerList *pLoggerFactory)
        {
            for (LoggerFactory::LoggerList::iterator iter = LoggerFactory::loggers_->begin();
                LoggerFactory::loggers_->end() != iter; ++iter)
            {
                JUDGER_SAFE_DELETE_OBJ_AND_RESET((*iter).second);
            }
        }
    };
};

}   // namespace IMUST

#endif  // IMUST_OJ_LOGGER_H

