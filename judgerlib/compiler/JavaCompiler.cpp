#include "stdafx.h"
#include "JavaCompiler.h"


namespace IMUST
{
    
namespace CompileArg
{
    const OJInt32_t LimitTime = 20000;
    const OJInt32_t LimitMemory = 128*1024*1024;

    const OJString Cmd = OJStr("javac -J-Xms32m -J-Xmx256m %s");
}


JavaCompiler::JavaCompiler(void)
{
}


JavaCompiler::~JavaCompiler(void)
{
}

void JavaCompiler::run(
    const OJString & codeFile,
    const OJString & exeFile,
    const OJString & compileFile)
{
    OJString cmdLine;
    FormatString(cmdLine, CompileArg::Cmd.c_str(), codeFile.c_str());

    ProcessPtr wp = ProcessFactory::create(ProcessType::Compiler, GetOJString(""), compileFile);
    wp->create(cmdLine, CompileArg::LimitTime, CompileArg::LimitMemory);
    result_ = wp->getResult();
}

}//namespace IMUST