#pragma once

#include "../judgerlib/platformlayer/PlatformLayer.h"
#include <memory>

namespace IMUST
{
namespace MatcherCode
{
    const OJInt32_t Success         = 0;
    const OJInt32_t WrongAnswer     = 1;
    const OJInt32_t PressentError   = 2;
    const OJInt32_t SystemError     = 3;
}

class IMatcher
{
public:
    IMatcher();
    virtual ~IMatcher();

    virtual bool run(const OJString & answerOutputFile, 
        const OJString & userOutputFile,
        bool specail = false,
        const OJString & specailMatcherInfo = OJStr("")) = 0;

    virtual bool isAccept();
    virtual bool isWrongAnswer();
    virtual bool isPresentError();
    virtual bool isSystemError();

protected:
    OJInt32_t result_ ;
};

typedef std::shared_ptr<IMatcher> MatcherPtr;

class MatcherFactory
{
public:
    MatcherFactory();
    virtual ~MatcherFactory();

    static MatcherPtr create(void);
};



}//namespace IMUST