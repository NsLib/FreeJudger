#include "stdafx.h"
#include "NormalMatcher.h"

namespace IMUST
{
    
namespace
{

const OJString WhiteSpaces = OJStr(" \r\n\t");

    
bool isWhiteSpace(OJChar_t ch)
{
    return WhiteSpaces.find(ch) != WhiteSpaces.npos;
}

}

NormalMatcher::NormalMatcher(void)
{
}


NormalMatcher::~NormalMatcher(void)
{
}

void NormalMatcher::run(
    const OJString & answerOutputFile, 
    const OJString & userOutputFile)
{
    result_ = compareFile(answerOutputFile, userOutputFile);
}

OJInt32_t NormalMatcher::compareFile(const OJString & srcFile, const OJString & destFile)
{
    OJString srcBuffer;
    if (!FileTool::ReadString(srcBuffer, srcFile))
    {
        return AppConfig::JudgeCode::SystemError;
    }

    OJString dstBuffer;
    if (!FileTool::ReadString(dstBuffer, destFile))
    {
        return AppConfig::JudgeCode::SystemError;
    }

    return compareString(srcBuffer, dstBuffer);
}

OJInt32_t NormalMatcher::compareString(const OJString & srcBuffer, const OJString & dstBuffer)
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
            return AppConfig::JudgeCode::WrongAnswer;
        }
    }

    while(i < srcLen)
    {
        if (!isWhiteSpace(srcBuffer[i]))
        {
            return AppConfig::JudgeCode::WrongAnswer;
        }
        presentError = true;
        ++i;
    }
        
    while(k < dstLen)
    {
        if (!isWhiteSpace(dstBuffer[k]))
        {
            return AppConfig::JudgeCode::WrongAnswer;
        }
        presentError = true;
        ++k;
    }

    if(presentError)
    {
        return AppConfig::JudgeCode::PresentError;
    }

    return AppConfig::JudgeCode::Accept;
}


}//namespace IMUST