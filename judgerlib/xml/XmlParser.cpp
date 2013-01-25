#include "XmlParser.h"

namespace IMUST
{
XmlParser::XmlParser() :
    doc_(new XmlDocumentType)
{

}

XmlParser::~XmlParser()
{

}

bool XmlParser::loadFile(const OJString &xmlFileName)
{

    return true;
}

bool XmlParser::getNodePropertyInt32(std::shared_ptr<XmlNodeType> node,
                                OJInt32_t &num) const
{

    return true;
}

bool XmlParser::getNodePropertyString(std::shared_ptr<XmlNodeType> node, 
                                    OJString &str) const
{
    return true;

}

bool XmlParser::getNodeContextInt32(std::shared_ptr<XmlNodeType> node,
                                    OJInt32_t &num) const
{
    return true;

}

bool XmlParser::getNodeContextString(std::shared_ptr<XmlNodeType> node, 
                                    OJString &str) const
{
    return true;
}




}   // namespace IMUST
