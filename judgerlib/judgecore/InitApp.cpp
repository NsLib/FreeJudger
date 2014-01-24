#include "stdafx.h"
#include "InitApp.h"

#include <Windows.h>

#include "../logger/Logger_log4cxx.h"

namespace IMUST
{

bool InitApp()
{
    if (!details::InitAppInitLogger())
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

#define REG_LOGGER  LoggerFactory::registerLogger(new Log4CxxLoggerImpl(GetOJString("log.cfg") 

    REG_LOGGER ,GetOJString("logger1")), LoggerId::Thread1LoggerId);
    REG_LOGGER ,GetOJString("logger2")), LoggerId::Thread2LoggerId);
    REG_LOGGER ,GetOJString("logger3")), LoggerId::Thread3LoggerId);
    REG_LOGGER ,GetOJString("logger4")), LoggerId::Thread4LoggerId);
    REG_LOGGER ,GetOJString("logger5")), LoggerId::Thread5LoggerId);
    REG_LOGGER ,GetOJString("logger6")), LoggerId::Thread6LoggerId);
    REG_LOGGER ,GetOJString("logger7")), LoggerId::Thread7LoggerId);
    REG_LOGGER ,GetOJString("logger8")), LoggerId::Thread8LoggerId);

#undef REG_LOGGER

    logger->logInfo(GetOJString("[Daemon] - IMUST::InitApp - Initialize application succeed"));

    return true;
}

namespace details
{

bool InitAppInitLogger()
{
    try
    {
        IMUST::LoggerFactory::registerLogger(
            new IMUST::Log4CxxLoggerImpl(GetOJString("log.cfg"), GetOJString("logger0")), 
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
