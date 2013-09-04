#include "Matcher.h"


namespace IMUST
{

IMatcher::IMatcher()
    : result_(MatcherCode::SystemError)
{
}

IMatcher::~IMatcher()
{
}

bool IMatcher::isAccept()
{
    return result_ == MatcherCode::Success;
}

bool IMatcher::isWrongAnswer()
{
    return result_ == MatcherCode::WrongAnswer;
}

bool IMatcher::isPresentError()
{
    return result_ == MatcherCode::PressentError;
}

bool IMatcher::isSystemError()
{
    return result_ == MatcherCode::SystemError;
}




}//namespace IMUST