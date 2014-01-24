#include "stdafx.h"
#include "SpecailMatcher.h"

namespace IMUST
{

SpecailMatcher::SpecailMatcher(const OJString & specialCode)
{
}


SpecailMatcher::~SpecailMatcher(void)
{
}

void SpecailMatcher::run(const OJString & answerOutputFile, 
        const OJString & userOutputFile)
{
    result_ = AppConfig::JudgeCode::SystemError;
}

}//namespace IMUST