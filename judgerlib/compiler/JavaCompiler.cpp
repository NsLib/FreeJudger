#include "JavaCompiler.h"
#include "../process/Process.h"
#include "../util/Utility.h"


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

bool JavaCompiler::run(
    const OJString & codeFile,
    const OJString & exeFile,
    const OJString & compileFile)
{
    OJString cmdLine;
    FormatString(cmdLine, CompileArg::Cmd.c_str(), codeFile.c_str());

    IMUST::WindowsProcess wp(GetOJString(""), compileFile);
    wp.create(cmdLine, CompileArg::LimitTime, CompileArg::LimitMemory);
    result_ = wp.getExitCodeEx();

    return isAccept();
}

}//namespace IMUST