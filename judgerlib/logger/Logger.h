
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

    virtual void logFatal(const OJString &msg) = 0;
    virtual void logError(const OJString &msg) = 0;
    virtual void logWarn(const OJString &msg) = 0;
    virtual void logInfo(const OJString &msg) = 0;
    virtual void logDebug(const OJString &msg) = 0;
    virtual void logTrace(const OJString &msg) = 0;
};

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

class JUDGER_API Logger
{
public:
    explicit Logger(ILogger *logger);
    ~Logger();

    Logger &get() { return *this; }
    void logFatal(const OJString &msg);
    void logError(const OJString &msg);
    void logWarn(const OJString &msg);
    void logInfo(const OJString &msg);
    void logDebug(const OJString &msg);
    void logTrace(const OJString &msg);

private:
    ILogger *logger_;
};

class LoggerFactory
{
    struct deleter
    {
        void operator()(LoggerFactory *pLoggerFactory)
        {
            for (LoggerList::iterator iter = LoggerFactory::loggers_->begin();
                LoggerFactory::loggers_->end() != iter; ++iter)
            {
                JUDGER_SAFE_DELETE_OBJ((*iter).second);
            }
        }
    };
public:
    typedef std::map<OJInt32_t, Logger *>               LoggerList;
    typedef std::shared_ptr<LoggerList>                 SharedLoggerList;

public:
    static Logger &getLogger(const OJInt32_t loggerId) throw ();
    static bool registerLogger(Logger *logger, const OJInt32_t loggerId) throw ();

private:
    LoggerFactory();
    ~LoggerFactory();

private:
    static SharedLoggerList loggers_;
};

}   // namespace IMUST

#endif  // IMUST_OJ_LOGGER_H

