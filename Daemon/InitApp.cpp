#include "InitApp.h"

#include <Windows.h>

#include "../judgerlib/logger/Logger.h"
#include "../judgerlib/logger/Logger_log4cxx.h"
#include "../judgerlib/config/AppConfig.h"

namespace IMUST
{

bool InitApp()
{
    if (!details::InitLogger())
    {
        MessageBoxW(NULL, L"初始化日志系统失败", L"错误", MB_OK);
        return false;
    }

    ILogger *logger = LoggerFactory::getLogger(LoggerId::AppInitLoggerId);
    logger->logTrace(GetOJString("[Daemon] - IMUST::InitApp"));

    if (!details::InitAppConfig())
    {
        MessageBoxW(NULL, L"初始化程序配置集失败，详情见日志", L"错误", MB_OK);
        return false;
    }

    logger->logInfo(GetOJString("[Daemon] - IMUST::InitApp - Initialize application succeed"));

    return true;
}

namespace details
{

bool InitLogger()
{
    try
    {
        IMUST::LoggerFactory::registerLogger(
            new IMUST::Log4CxxLoggerImpl(GetOJString("log.cfg"), GetOJString("logger1")), 
            IMUST::LoggerId::AppInitLoggerId);
    }
    catch (...)
    {
        return false;
    }

    return true;
}

bool InitAppConfig()
{
    ::IMUST::LoggerFactory::getLogger(LoggerId::AppInitLoggerId)->logTrace(
        GetOJString("[Daemon] - IMUST::details::InitAppConfig"));
    return ::IMUST::AppConfig::InitAppConfig();
}

}   // namespace details





}   // namespace IMUST
