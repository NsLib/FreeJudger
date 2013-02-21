#pragma once
#include "matcher.h"

#include <vector>

namespace IMUST
{

class NormalMatcher : public IMatcher
{
public:
    NormalMatcher(void);
    virtual ~NormalMatcher(void);

    virtual bool run(const OJString & answerOutputFile, 
        const OJString & userOutputFile);

    OJInt32_t compare(const OJString & srcFile, const OJString & destFile);

    OJInt32_t compare(std::vector<OJChar_t> & srcBuffer, std::vector<OJChar_t> & dstBuffer);

};

}//namespace IMUST