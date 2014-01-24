#pragma once

namespace IMUST
{

class IMatcher
{
public:
    IMatcher();
    virtual ~IMatcher();

    virtual void run(const OJString & answerOutputFile, 
        const OJString & userOutputFile) = 0;

    OJInt32_t getResult() const { return result_;}

protected:
    OJInt32_t result_ ;
};

typedef std::shared_ptr<IMatcher> MatcherPtr;

class MatcherFactory
{
public:
    MatcherFactory();
    virtual ~MatcherFactory();

    static MatcherPtr create(bool isSpecail, const OJString & specialCode);
};



}//namespace IMUST