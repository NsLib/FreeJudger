#include "Matcher.h"
#include "../judgerlib/filetool/FileTool.h"


namespace IMUST
{
namespace
{
    const OJString WhiteSpaces = OJStr(" \r\n\t");
}

IMatcher::IMatcher()
    : result_(MatcherCode::SystemError)
{
}

IMatcher::~IMatcher()
{
}

bool IMatcher::isAccept()
{
    return result_ == MatcherCode::Success;
}

bool IMatcher::isWrongAnswer()
{
    return result_ == MatcherCode::WrongAnswer;
}

bool IMatcher::isPresentError()
{
    return result_ == MatcherCode::PressentError;
}

bool IMatcher::isSystemError()
{
    return result_ == MatcherCode::SystemError;
}


class NormalMatcher : public IMatcher
{
public:
    NormalMatcher()
    {
    }
    ~NormalMatcher()
    {
    }

    virtual bool run(
        const OJString & answerOutputFile, 
        const OJString & userOutputFile,
        bool specail/* = false*/,
        const OJString & specailMatcherInfo/* = OJStr("")*/)
    {
        if(specail)
        {
            result_ = MatcherCode::SystemError;
        }
        else 
        {
            result_ = compare(answerOutputFile, userOutputFile);
        }

        return isAccept();
    }

    OJInt32_t compare(const OJString & srcFile, const OJString & destFile)
    {
        std::vector<OJChar_t> srcBuffer;
        if (!FileTool::ReadFile(srcBuffer, srcFile, false))
        {
            return MatcherCode::SystemError;
        }

        std::vector<OJChar_t> dstBuffer;
        if (!FileTool::ReadFile(dstBuffer, destFile, false))
        {
            return MatcherCode::SystemError;
        }

        return compare(srcBuffer, dstBuffer);
    }

    bool isWhiteSpace(OJChar_t ch)
    {
        return ch==OJCh('\0') || (WhiteSpaces.find(ch) != WhiteSpaces.npos);
    }

    OJInt32_t compare(std::vector<OJChar_t> & srcBuffer, std::vector<OJChar_t> & dstBuffer)
    {
        bool presentError = false;

        OJUInt32_t srcLen = srcBuffer.size();
        OJUInt32_t dstLen = dstBuffer.size();

        OJChar_t srcChr, dstChr;

        OJUInt32_t i=0, k=0;
        while(i<srcLen && k<dstLen)
        {
            srcChr = srcBuffer[i];
            dstChr = dstBuffer[k];

            if (srcChr == dstChr)
            {
                ++i;
                ++k;
                continue;
            }

            //srcChr != dstChr

            if (srcChr == OJCh('\r') && dstChr == OJCh('\n'))
            {
                ++i;
            }
            else if (dstChr == OJCh('\r') && srcChr == OJCh('\n'))
            {
                ++k;
            }
            else if (isWhiteSpace(srcChr))
            {
                presentError = true;
                ++i;
            }
            else if (isWhiteSpace(dstChr))
            {
                presentError = true;
                ++k;
            }
            else
            {
                return MatcherCode::WrongAnswer;
            }
        }

        while(i < srcLen)
        {
            if (!isWhiteSpace(srcBuffer[i]))
            {
                return MatcherCode::WrongAnswer;
            }
            presentError = true;
            ++i;
        }
        
        while(k < dstLen)
        {
            if (!isWhiteSpace(dstBuffer[k]))
            {
                return MatcherCode::WrongAnswer;
            }
            presentError = true;
            ++k;
        }

        if(presentError)
        {
            return MatcherCode::PressentError;
        }

        return MatcherCode::Success;
    }

};


MatcherFactory::MatcherFactory()
{
}

MatcherFactory::~MatcherFactory()
{
}

/*static */MatcherPtr MatcherFactory::create(void)
{
    return MatcherPtr(new NormalMatcher());
}

}//namespace IMUST