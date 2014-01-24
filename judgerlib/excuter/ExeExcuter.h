#pragma once

#include "excuter.h"

namespace IMUST
{

class ExeExcuter :
    public IExcuter
{
public:
    ExeExcuter(void);
    virtual ~ExeExcuter(void);

    virtual void run(
        const OJString & exeFile,
        const OJString & inputFile,
        const OJString & outputFile,
        OJInt32_t limitTime,
        OJInt32_t limitMemory
        );
};

}//namespace IMUST