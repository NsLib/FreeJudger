#pragma once


namespace IMUST
{

class IExcuter
{
public:
    IExcuter();
    virtual ~IExcuter();

    virtual void run(
        const OJString & exeFile,
        const OJString & intputFile,
        const OJString & outputFile,
        OJInt32_t limitTime,
        OJInt32_t limitMemory
        ) = 0;

    OJInt32_t getResult() const { return result_;}
    OJInt32_t getRunMemory() const { return runMemory_ ; }
    OJInt32_t getRunTime() const { return runTime_; }

protected:

    OJInt32_t runTime_ ;
    OJInt32_t runMemory_;
    OJInt32_t result_;

};

typedef std::shared_ptr<IExcuter> ExcuterPtr;

class ExcuterFactory
{
public:
    ExcuterFactory();
    ~ExcuterFactory();

    static ExcuterPtr create(OJInt32_t language);
};

}//namespace IMUST