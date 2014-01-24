#include "stdafx.h"
#include "Compiler.h"


namespace IMUST
{
    
namespace CompileArg
{
    const OJInt32_t limitTime = 20000;
    const OJInt32_t limitMemory = 32*1024*1024;

    const OJInt32_t javaLimitTime = 20000;
    const OJInt32_t javaLimitMemory = 128*1024*1024;

    const OJString gcc = OJStr("gcc %s -o %s -O2 -Wall -lm --static -std=c99 -DONLINE_JUDGE");
    const OJString gPlus = OJStr("g++ %s -o %s -O2 -Wall -lm --static -DONLINE_JUDGE");
    const OJString java = OJStr("javac -J-Xms32m -J-Xmx256m %s");
}

ICompiler::ICompiler()
    : result_(ProcessExitCode::SystemError)
{
}
ICompiler::~ICompiler()
{
}

bool ICompiler::isAccept()
{
    return result_ == ProcessExitCode::Success;
}
bool ICompiler::isSystemError()
{
    return result_ == ProcessExitCode::SystemError;
}
bool ICompiler::isCompileError()
{
    return result_ == ProcessExitCode::RuntimeError;
}



}//namespace IMUST