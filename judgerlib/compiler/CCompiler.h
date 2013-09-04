#pragma once

#include "compiler.h"

namespace IMUST
{

class CCompiler : public ICompiler
{
public:
    CCompiler(void);
    virtual ~CCompiler(void);

    bool CCompiler::run(
        const OJString & codeFile,
        const OJString & exeFile,
        const OJString & compileFile);
};

}