#include "Utility.h"

namespace IMUST
{

namespace
{

OJString String2OJStringImpl(const char *str)
{
    OJString ojStr;

    if (0 == str)
        return ojStr;
 
    size_t ojStrLen = mbstowcs(0, str, 0);

    if (ojStrLen ==(size_t)(-1))  
        return ojStr;

    ojStrLen++;

    OJChar_t *buf  = new OJChar_t[ojStrLen];  
    mbstowcs(buf, str, ojStrLen);
    ojStr = buf;
    delete buf;

    return ojStr;  
}

}   // namespace

OJString String2OJString(const char *str)
{
    return String2OJStringImpl(str);
}

OJString String2OJString(const std::string str)
{
    return String2OJStringImpl(str.c_str());
}


bool FormatStringVS(OJString & dest, const OJChar_t * pFormat, va_list pArgList)
{
    int len = _vscwprintf(pFormat, pArgList) + 1;
    if(len < 0)
    {
        return false;
    }
    dest.resize(len);

    len = vswprintf(&dest[0], pFormat, pArgList);
    
    if(len < 0)
    {
        return false;
    }

    dest.erase(len);
    return true;
}

bool FormatString(OJString & dest, const OJChar_t * pFormat, ...)
{
    va_list pArgList;
    va_start (pArgList, pFormat);

    bool ret = FormatStringVS(dest, pFormat, pArgList);

    va_end(pArgList);

    return ret;
}

void DebugMessage(const OJChar_t * pFormat, ...)
{
    OJString dest;
    va_list pArgList;
    va_start (pArgList, pFormat);

    if(FormatStringVS(dest, pFormat, pArgList))
    {
        DEBUG_MSG(dest.c_str());
    }

    va_end(pArgList);
}

}   // namespace IMUST

