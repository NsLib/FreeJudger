#pragma once
#include "compiler.h"

namespace IMUST
{

class JavaCompiler :
    public ICompiler
{
public:
    JavaCompiler(void);
    virtual ~JavaCompiler(void);

    virtual void run(
        const OJString & codeFile,
        const OJString & exeFile,
        const OJString & compileFile);
};

}//namespace IMUST