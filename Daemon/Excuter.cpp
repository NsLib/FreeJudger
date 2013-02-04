#include "Excuter.h"


namespace IMUST
{
IExcuter::IExcuter()
    : runTime_(0)
    , runMemory_(0)
    , result_(0)
{
}

IExcuter::~IExcuter()
{
}


bool IExcuter::isAccept()
{
    return false;
}

bool IExcuter::isTimeOutOfLimited()
{
    return false;
}

bool IExcuter::isMemoryOutOfLimited()
{
    return false;
}

bool IExcuter::isOutputOutOfLimited()
{
    return false;
}

bool IExcuter::isRuntimeError()
{
    return false;
}

bool IExcuter::isSystemError()
{
    return true;
}




ExcuterFactory::ExcuterFactory()
{
}
ExcuterFactory::~ExcuterFactory()
{
}

/*static */ExcuterPtr ExcuterFactory::create(OJInt32_t language)
{
    return NULL;
}

}//namespace IMUST