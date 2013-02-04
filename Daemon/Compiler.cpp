#include "Compiler.h"

#include "../judgerlib/config/AppConfig.h"
#include "../judgerlib/process/Process.h"


namespace IMUST
{
    
namespace CompileArg
{
    const OJInt32_t limitTime = 20000;
    const OJInt32_t limitMemory = 128*1024;

    const OJString gcc = OJStr("gcc %s -o %s");
    const OJString gPlus = OJStr("g++ %s -o %s");
    const OJString java = OJStr("javac %s");
}


class CCompiler : public ICompiler
{
public:
    virtual OJInt32_t run(
        const OJString & codeFile,
        const OJString & exeFile,
        const OJString & compileFile)
    {
        OJChar_t buffer[1024];
        OJSprintf(buffer, CompileArg::gcc.c_str(), codeFile.c_str(), exeFile.c_str());

        IMUST::WindowsProcess wp(OJStr(""), compileFile);
        wp.create(buffer, CompileArg::limitTime, CompileArg::limitMemory);
        return wp.getExitCode();
    }
};

class CppCompiler : public ICompiler
{
public:
    virtual OJInt32_t run(
        const OJString & codeFile,
        const OJString & exeFile,
        const OJString & compileFile)
    {
        OJChar_t buffer[1024];
        OJSprintf(buffer, CompileArg::gPlus.c_str(), codeFile.c_str(), exeFile.c_str());

        IMUST::WindowsProcess wp(GetOJString(""), compileFile);
        wp.create(buffer, CompileArg::limitTime, CompileArg::limitMemory);
        return wp.getExitCode();
    }
};

class JavaCompiler : public ICompiler
{
public:
    virtual OJInt32_t run(
        const OJString & codeFile,
        const OJString & exeFile,
        const OJString & compileFile)
    {
        OJChar_t buffer[1024];
        OJSprintf(buffer, CompileArg::java.c_str(), codeFile.c_str());

        IMUST::WindowsProcess wp(GetOJString(""), compileFile);
        wp.create(buffer, CompileArg::limitTime, CompileArg::limitMemory);
        return wp.getExitCode();
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
    if(language == 0)
    {
        return CompilerPtr(new CCompiler());
    }
    else if(language == 1)
    {
        return CompilerPtr(new CppCompiler());
    }
    else if(language == 2)
    {
        return CompilerPtr(new JavaCompiler());
    }

    assert(false && "unknown language!");

    return NULL;
}

}//namespace IMUST