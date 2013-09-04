#pragma once

#include "../platformlayer/PlatformLayer.h"
#include <memory>

namespace IMUST
{

class IExcuter
{
public:
    IExcuter();
    virtual ~IExcuter();

    virtual bool run(
        const OJString & exeFile,
        const OJString & intputFile,
        const OJString & outputFile,
        OJInt32_t limitTime,
        OJInt32_t limitMemory
        ) = 0;

    bool isAccept();
    bool isTimeOutOfLimited();
    bool isMemoryOutOfLimited();
    bool isOutputOutOfLimited();
    bool isRuntimeError();
    bool isSystemError();

    OJInt32_t getRunMemory(){ return runMemory_ ; }
    OJInt32_t getRunTime(){ return runTime_; }

protected:

    OJInt32_t runTime_ ;
    OJInt32_t runMemory_;
    OJInt32_t result_;

};

typedef std::shared_ptr<IExcuter> ExcuterPtr;

class ExcuterFactory
{
public:
    ExcuterFactory();
    ~ExcuterFactory();

    static ExcuterPtr create(OJInt32_t language);
};

}//namespace IMUST