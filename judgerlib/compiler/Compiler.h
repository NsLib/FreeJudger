#pragma once

#include "../platformlayer/PlatformLayer.h"
#include <memory>

namespace IMUST
{

class ICompiler
{
public:
    ICompiler();
    virtual ~ICompiler();

    virtual bool run(
        const OJString & codeFile,
        const OJString & exeFile,
        const OJString & compileFile) = 0;

    bool isAccept();
    bool isSystemError();
    bool isCompileError();
protected:
    OJInt32_t result_;
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