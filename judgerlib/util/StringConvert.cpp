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

}   // namespace IMUST

