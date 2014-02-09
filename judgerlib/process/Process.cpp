#include "stdafx.h"
#include "Process.h"
#include "../util/Watch.h"

namespace IMUST
{
    namespace
    {
        OJInt32_t NumProcess = 0;
    }

IProcess::IProcess()
{
    WatchTool::WatchCount(OJStr("core/numProcess"), NumProcess, +1);
}

IProcess::~IProcess()
{
    WatchTool::WatchCount(OJStr("core/numProcess"), NumProcess, -1);
}

}   // namespace IMUST