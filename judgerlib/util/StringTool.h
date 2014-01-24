#pragma once

#ifndef IMUST_OJ_STRING_TOOL_H
#define IMUST_OJ_STRING_TOOL_H

#include "../platformlayer/PlatformLayer.h"

namespace IMUST
{

    const OJString StrTrue = OJStr("true");
    const OJString StrFalse = OJStr("false");

    bool OJString2UTF8(std::string & strDst, const OJString & strSrc);
    bool UTF82OJString(OJString & strDst, const std::string & srtSrc);

    bool FormatStringVS(OJString & dest, const OJChar_t * pFormat, va_list pArgList);

    bool FormatString(OJString & dest, const OJChar_t * pFormat, ...);

    void DebugMessage(const OJChar_t * pFormat, ...);

#define DEBUG_MSG_VS DebugMessage


}//end namespace IMUST

#endif //IMUST_OJ_STRING_TOOL_H