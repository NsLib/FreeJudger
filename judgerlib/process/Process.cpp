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
    WatchTool::LockRoot();
    ++NumProcess;
    WatchTool::Root()->watch(OJStr("core/numProcess"), NumProcess);
    WatchTool::UnlockRoot();
}

IProcess::~IProcess()
{
    WatchTool::LockRoot();
    --NumProcess;
    WatchTool::Root()->watch(OJStr("core/numProcess"), NumProcess);
    WatchTool::UnlockRoot();
}

}   // namespace IMUST