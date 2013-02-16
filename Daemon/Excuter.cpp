#include "Excuter.h"

#include "../judgerlib/process/Process.h"
#include "../judgerlib/filetool/FileTool.h"


namespace IMUST
{
IExcuter::IExcuter()
    : runTime_(0)
    , runMemory_(0)
    , result_(ProcessExitCode::SystemError)
{
}

IExcuter::~IExcuter()
{
}


bool IExcuter::isAccept()
{
    return result_ == ProcessExitCode::Success ;
}

bool IExcuter::isTimeOutOfLimited()
{
    return result_ == ProcessExitCode::TimeLimited;
}

bool IExcuter::isMemoryOutOfLimited()
{
    return result_ == ProcessExitCode::MemoryLimited;
}

bool IExcuter::isOutputOutOfLimited()
{
    return false;
}

bool IExcuter::isRuntimeError()
{
    return result_ == ProcessExitCode::RuntimeError;
}

bool IExcuter::isSystemError()
{
    return result_ == ProcessExitCode::SystemError;
}


class ExeExcuter : public IExcuter
{
public:
    ExeExcuter(){}
    ~ExeExcuter(){}

    virtual bool run(
        const OJString & exeFile,
        const OJString & inputFile,
        const OJString & outputFile,
        OJInt32_t limitTime,
        OJInt32_t limitMemory
        )
    {
        IMUST::WindowsProcess wp(inputFile, outputFile);
        wp.create(exeFile, limitTime, limitMemory);
        result_ = wp.getExitCodeEx();

        runTime_ = wp.getRunTime();
        runMemory_ = wp.getRunMemory();

        return isAccept();
    }

};

class JavaExcuter : public IExcuter
{
public:
    JavaExcuter(){}
    ~JavaExcuter(){}

    virtual bool run(
        const OJString & exeFile,
        const OJString & inputFile,
        const OJString & outputFile,
        OJInt32_t limitTime,
        OJInt32_t limitMemory
        )
    {
        OJString exePath = FileTool::GetFilePath(exeFile);
        OJString exeFileName = FileTool::GetFileName(exeFile);//get only name

        OJChar_t buffer[1024];
        OJSprintf(buffer, OJStr("java -cp %s %s"), exePath.c_str(), exeFileName.c_str());

        IMUST::WindowsProcess wp(inputFile, outputFile);
        wp.create(buffer, limitTime, limitMemory);
        result_ = wp.getExitCodeEx();

        runTime_ = wp.getRunTime();
        runMemory_ = wp.getRunMemory();

        return isAccept();
    }

};



ExcuterFactory::ExcuterFactory()
{
}
ExcuterFactory::~ExcuterFactory()
{
}

/*static */ExcuterPtr ExcuterFactory::create(OJInt32_t language)
{
    return ExcuterPtr(new ExeExcuter());
}

}//namespace IMUST