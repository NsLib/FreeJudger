#include "stdafx.h"
#include "InitApp.h"

#include <Windows.h>

#include "../logger/Logger_log4cxx.h"

namespace
{
    bool g_AppValid = false;
}

namespace IMUST
{

bool IsAppValid()
{
    return g_AppValid;
}

bool InitApp()
{
    if(g_AppValid) return false;

    OJString path = FileTool::GetModulePath();
    if(!FileTool::SetCurPath(path))
    {
        MessageBoxW(NULL, L"设置当前路径失败！", L"错误", MB_ICONSTOP);
        return false;
    }

    if (!details::InitAppInitLogger())
    {
        MessageBoxW(NULL, L"初始化日志系统失败", L"错误", MB_ICONSTOP);
        return false;
    }

    ILogger *logger = LoggerFactory::getLogger(LoggerId::AppInitLoggerId);
    logger->logTrace(GetOJString("IMUST::InitApp"));

    if (!details::InitAppConfig())
    {
        MessageBoxW(NULL, L"初始化程序配置集失败，详情见日志", L"错误", MB_ICONSTOP);
        return false;
    }

    for(OJInt32_t i=0; i<AppConfig::CpuInfo::NumberOfCore; ++i)
    {
        OJInt32_t threadLogId = i + 1;
        OJString strTag;
        FormatString(strTag, OJStr("logger%d"), threadLogId);

        LoggerFactory::registerLogger(new Log4CxxLoggerImpl(OJStr("log.cfg"), strTag), threadLogId);
    }

    //hook操作
    if(NULL == LoadLibraryW(L"windowsapihook.dll"))
    {
        OJString msg;
        FormatString(msg, OJStr("Load windowsapihook.dll faild! - error code:%u"), GetLastError());
        logger->logWarn(msg);

        ::MessageBoxW(NULL, msg.c_str(), L"ERROR", MB_ICONSTOP);
        return false;
    }

    ::MessageBoxW(NULL, L"你不应该看到这个MessageBox", L"ERROR", MB_ICONSTOP);
    ::MessageBoxA(NULL, "你不应该看到这个MessageBox", "ERROR", MB_ICONSTOP);

    logger->logInfo(GetOJString("IMUST::InitApp - Initialize application succeed"));

    g_AppValid = true;
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
        GetOJString("IMUST::details::InitAppConfig"));
    return ::IMUST::AppConfig::InitAppConfig();
}

}   // namespace details





}   // namespace IMUST
