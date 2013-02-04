#pragma once

#include "../judgerlib/platformlayer/PlatformLayer.h"
#include <memory>

namespace IMUST
{

class ICompiler
{
public:
    virtual OJInt32_t run(
        const OJString & codeFile,
        const OJString & exeFile,
        const OJString & compileFile) = 0;
};

typedef std::shared_ptr<ICompiler> CompilerPtr;

class CompilerFactory
{
public:
    CompilerFactory();
    ~CompilerFactory();

    static CompilerPtr create(OJInt32_t language);
};

}//namspace IMUST