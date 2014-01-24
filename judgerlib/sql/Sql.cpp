#include "Sql.h"
#include "../util/StringTool.h"

namespace IMUST
{


SqlVar::SqlVar(const OJString & v)
    : value_(v)
{}

bool SqlVar::getBool() const
{
    if(value_ == StrTrue)
    {
        return true;
    }
    return getInt32() != 0;
}

OJInt32_t SqlVar::getInt32() const
{
    OJInt32_t d = 0;
    OJSscanf(value_.c_str(), OJStr("%d"), &d);
    return d;
}

OJFloat16_t SqlVar::getFloat16() const
{
    OJFloat16_t d = 0;
    OJSscanf(value_.c_str(), OJStr("%f"), &d);
    return d;
}

OJFloat32_t SqlVar::getFloat32() const
{
    OJFloat32_t d = 0;
    OJSscanf(value_.c_str(), OJStr("%lf"), &d);
    return d;
}


const SqlVar & SqlRow::operator[](OJUInt32_t index) const
{
    return getVar(index);
}

} // namespace IMUST