
#ifndef IMUST_OJ_LOGGER_LOG4CXX_H
#define	IMUST_OJ_LOGGER_LOG4CXX_H

#include "Logger.h"

namespace IMUST
{
// log4cxx的LoggerPtr无法使用前置声明，因此要做包装
class LoggerPtrWrapper;

class JUDGER_API Log4CxxLoggerImpl : public ILogger
{
public:
    Log4CxxLoggerImpl(const OJString &configFileName, 
        const OJString &logTag);
    virtual ~Log4CxxLoggerImpl();

    virtual void logFatal(const OJString &msg) const;
    virtual void logError(const OJString &msg) const;
    virtual void logWarn(const OJString &msg) const;
    virtual void logInfo(const OJString &msg) const;
    virtual void logDebug(const OJString &msg) const;
    virtual void logTrace(const OJString &msg) const;

private:
    LoggerPtrWrapper            *logger_;
};

}

#endif  // IMUST_OJ_LOGGER_LOG4CXX_H
