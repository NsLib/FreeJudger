#include "Excuter.h"

#include "../process/Process.h"


namespace IMUST
{
IExcuter::IExcuter()
    : runTime_(0)
    , runMemory_(0)
    , result_(ProcessExitCode::SystemError)
{
}

IExcuter::~IExcuter()
{
}


bool IExcuter::isAccept()
{
    return result_ == ProcessExitCode::Success ;
}

bool IExcuter::isTimeOutOfLimited()
{
    return result_ == ProcessExitCode::TimeLimited;
}

bool IExcuter::isMemoryOutOfLimited()
{
    return result_ == ProcessExitCode::MemoryLimited;
}

bool IExcuter::isOutputOutOfLimited()
{
    return false;
}

bool IExcuter::isRuntimeError()
{
    return result_ == ProcessExitCode::RuntimeError;
}

bool IExcuter::isSystemError()
{
    return result_ == ProcessExitCode::SystemError;
}


}//namespace IMUST