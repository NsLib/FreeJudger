#include "stdafx.h"
#include "Matcher.h"
#include "NormalMatcher.h"
#include "SpecailMatcher.h"

namespace IMUST
{


MatcherFactory::MatcherFactory()
{
}

MatcherFactory::~MatcherFactory()
{
}

/*static */MatcherPtr MatcherFactory::create(bool isSpecail, const OJString & specialCode)
{
    if(!isSpecail)
    {
        return MatcherPtr(new NormalMatcher());
    }

    return MatcherPtr(new SpecailMatcher(specialCode));
}

}//namespace IMUST