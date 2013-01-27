
#include "Xml.h"

#pragma warning(push)
#pragma warning(disable:4996)   // Conditional expression is constant

namespace IMUST
{

IXml::IXml()
{
}

IXml::~IXml()
{
}

void IXml::getInt32(OJInt32_t &d) const
{
    OJSscanf(cvalue(), OJStr("%d"), &d);
}

void IXml::getFloat16(OJFloat16_t &d) const
{
    OJSscanf(cvalue(), OJStr("%f"), &d);
}

void IXml::getFloat32(OJFloat32_t &d) const
{
    OJSscanf(cvalue(), OJStr("%lf"), &d);
}

void IXml::setInt32(OJInt32_t d)
{
    OJString buffer(16, OJCh('\0'));
    OJSprintf(&buffer[0], OJStr("%d"), d);
    setValue(buffer);
}

void IXml::setFloat16(OJFloat16_t d)
{
    OJString buffer(16, OJCh('\0'));
    OJSprintf(&buffer[0], OJStr("%f"), d);
    setValue(buffer);
}

void IXml::setFloat32(OJFloat32_t d)
{
    OJString buffer(32, OJCh('\0'));
    OJSprintf(&buffer[0], OJStr("%lf"), d);
    setValue(buffer);
}

bool IXml::readInt32(const OJString &tag, OJInt32_t & d) const
{
    XmlPtr node = read(tag);
    if(node)
    {
        node->getInt32(d);
        return true;
    }
    return false;
}

bool IXml::readFloat16(const OJString &tag, OJFloat16_t &d) const
{
    XmlPtr node = read(tag);
    if(node)
    {
        node->getFloat16(d);
        return true;
    }
    return false;
}
bool IXml::readFloat32(const OJString &tag, OJFloat32_t &d) const
{
    XmlPtr node = read(tag);
    if(node)
    {
        node->getFloat32(d);
        return true;
    }
    return false;
}

bool IXml::readString(const OJString &tag, OJString &d) const
{
    XmlPtr node = read(tag);
    if(node)
    {
        d = node->value();
        return true;
    }
    return false;
}

void IXml::writeInt32(const OJString &tag, OJInt32_t d)
{
    write(tag)->setInt32(d);
}

void IXml::writeFloat16(const OJString &tag, OJFloat16_t d)
{
    write(tag)->setFloat16(d);
}

void IXml::writeFloat32(const OJString &tag, OJFloat32_t d)
{
    write(tag)->setFloat32(d);
}

void IXml::writeString(const OJString &tag, const OJString & d)
{
    write(tag)->setValue(d);
}

bool IXml::reads(const OJString & tag, XmlPtrVector & vector)
{
    vector.clear();
    XmlPtr ptr = read(tag);
    while(ptr)
    {
        vector.push_back(ptr);
        ptr = ptr->getNextSibling();
    }
    return !vector.empty();
}

}// namespace IMUST

#pragma warning(pop)