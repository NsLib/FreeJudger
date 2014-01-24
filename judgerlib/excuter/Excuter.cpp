#include "stdafx.h"
#include "Excuter.h"


namespace IMUST
{
IExcuter::IExcuter()
    : runTime_(0)
    , runMemory_(0)
    , result_(AppConfig::JudgeCode::SystemError)
{
}

IExcuter::~IExcuter()
{
}

}//namespace IMUST