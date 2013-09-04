#include "SpecailMatcher.h"

namespace IMUST
{

SpecailMatcher::SpecailMatcher(const OJString & specialCode)
{
}


SpecailMatcher::~SpecailMatcher(void)
{
}

bool SpecailMatcher::run(const OJString & answerOutputFile, 
        const OJString & userOutputFile)
{
    result_ = MatcherCode::SystemError;

    return isAccept();
}

}//namespace IMUST