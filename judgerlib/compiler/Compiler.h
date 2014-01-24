#pragma once

namespace IMUST
{

class ICompiler
{
public:
    ICompiler();
    virtual ~ICompiler();

    virtual void run(
        const OJString & codeFile,
        const OJString & exeFile,
        const OJString & compileFile) = 0;

    OJInt32_t getResult() const { return result_; }

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