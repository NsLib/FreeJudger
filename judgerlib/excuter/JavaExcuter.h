#pragma once
#include "excuter.h"

namespace IMUST
{

class JavaExcuter :
    public IExcuter
{
public:
    JavaExcuter(void);
    virtual ~JavaExcuter(void);

    virtual void run(
        const OJString & exeFile,
        const OJString & intputFile,
        const OJString & outputFile,
        OJInt32_t limitTime,
        OJInt32_t limitMemory
        );
};


}//namespace IMUST