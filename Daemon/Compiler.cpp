#include "Compiler.h"

#include "../judgerlib/config/AppConfig.h"
#include "../judgerlib/process/Process.h"


namespace IMUST
{
    
namespace CompileArg
{
    const OJInt32_t limitTime = 20000;
    const OJInt32_t limitMemory = 32*1024*1024;

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


class CCompiler : public ICompiler
{
public:
    virtual bool run(
        const OJString & codeFile,
        const OJString & exeFile,
        const OJString & compileFile)
    {
        OJChar_t buffer[1024];
        OJSprintf(buffer, CompileArg::gcc.c_str(), codeFile.c_str(), exeFile.c_str());

        IMUST::WindowsProcess wp(OJStr(""), compileFile);
        wp.create(buffer, CompileArg::limitTime, CompileArg::limitMemory);
        result_ = wp.getExitCodeEx();

        return isAccept();
    }
};

class CppCompiler : public ICompiler
{
public:
    virtual bool run(
        const OJString & codeFile,
        const OJString & exeFile,
        const OJString & compileFile)
    {
        OJChar_t buffer[1024];
        OJSprintf(buffer, CompileArg::gPlus.c_str(), codeFile.c_str(), exeFile.c_str());

        IMUST::WindowsProcess wp(GetOJString(""), compileFile);
        wp.create(buffer, CompileArg::limitTime, CompileArg::limitMemory);
        result_ = wp.getExitCodeEx();

        return isAccept();
    }
};

class JavaCompiler : public ICompiler
{
public:
    virtual bool run(
        const OJString & codeFile,
        const OJString & exeFile,
        const OJString & compileFile)
    {
        OJChar_t buffer[1024];
        OJSprintf(buffer, CompileArg::java.c_str(), codeFile.c_str());

        IMUST::WindowsProcess wp(GetOJString(""), compileFile);
        wp.create(buffer, CompileArg::limitTime, CompileArg::limitMemory);
        result_ = wp.getExitCodeEx();

        return isAccept();
    }
};

CompilerFactory::CompilerFactory()
{
}

CompilerFactory::~CompilerFactory()
{
}

/*static */CompilerPtr CompilerFactory::create(OJInt32_t language)
{
    if(language == AppConfig::Language::C)
    {
        return CompilerPtr(new CCompiler());
    }
    else if(language == AppConfig::Language::Cxx)
    {
        return CompilerPtr(new CppCompiler());
    }
    else if(language == AppConfig::Language::Java)
    {
        return CompilerPtr(new JavaCompiler());
    }

    assert(false && "unsupport code language!");

    return NULL;
}

}//namespace IMUST