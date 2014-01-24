#pragma once
#include "matcher.h"

namespace IMUST
{

class NormalMatcher : public IMatcher
{
public:
    NormalMatcher(void);
    virtual ~NormalMatcher(void);

    virtual void run(const OJString & answerOutputFile, 
        const OJString & userOutputFile);

    OJInt32_t compareFile(const OJString & srcFile, const OJString & destFile);
    OJInt32_t compareString(const OJString & srcBuffer, const OJString & dstBuffer);

};

}//namespace IMUST