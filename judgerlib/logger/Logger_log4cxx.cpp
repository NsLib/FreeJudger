#include "Logger_log4cxx.h"

#include "../../thirdpartylib/log4cxx/log4cxx.h"
#include "../../thirdpartylib/log4cxx/propertyconfigurator.h"

namespace IMUST
{

class LoggerPtrWrapper
{
public:
    log4cxx::LoggerPtr &operator ->()
    {
        return loggerPtr_;
    }
    log4cxx::LoggerPtr &operator *()
    {
        return loggerPtr_;
    }
    log4cxx::LoggerPtr &operator =(const log4cxx::LoggerPtr loggerPtr)
    {
        loggerPtr_ = loggerPtr;
        return loggerPtr_;
    }

private:
    log4cxx::LoggerPtr  loggerPtr_;
};

Log4CxxLoggerImpl::Log4CxxLoggerImpl(const OJString &configFileName,
    const OJString &logTag) :
    logger_(new LoggerPtrWrapper)
{
    assert(!configFileName.empty() && "Config filename can not be empty");

    log4cxx::PropertyConfigurator::configure(configFileName);
    *logger_ = log4cxx::Logger::getLogger(logTag);
}

Log4CxxLoggerImpl::~Log4CxxLoggerImpl()
{

}

void Log4CxxLoggerImpl::logFatal(const OJString &msg) const
{
    (*logger_)->fatal(msg);

    DEBUG_MSG(msg.c_str());
}

void Log4CxxLoggerImpl::logError(const OJString &msg) const
{
    //TODO: 移除ERROR前缀。
    OJString str = OJStr("ERROR: ");
    str += msg;
    (*logger_)->error(str);

    DEBUG_MSG(str.c_str());
}

void Log4CxxLoggerImpl::logWarn(const OJString &msg) const
{
    (*logger_)->warn(msg);

    DEBUG_MSG(msg.c_str());
}

void Log4CxxLoggerImpl::logInfo(const OJString &msg) const
{
    (*logger_)->info(msg);

    DEBUG_MSG(msg.c_str());
}

void Log4CxxLoggerImpl::logDebug(const OJString &msg) const
{
    (*logger_)->debug(msg);

    DEBUG_MSG(msg.c_str());
}

void Log4CxxLoggerImpl::logTrace(const OJString &msg) const
{
    (*logger_)->trace(msg);

    DEBUG_MSG(msg.c_str());
}

}