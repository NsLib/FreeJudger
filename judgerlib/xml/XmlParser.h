
#ifndef IMUST_OJ_XML_PARSER_H
#define IMUST_OJ_XML_PARSER_H

#include <memory>

#include "../platformlayer/PlatformLayer.h"

#include "../../thirdpartylib/rapidxml/rapidxml.hpp"
#include "../../thirdpartylib/rapidxml/rapidxml_utils.hpp"

namespace IMUST
{

class JUDGER_API XmlParser
{
public:
    typedef rapidxml::xml_document<OJChar_t>    XmlDocumentType;
    typedef rapidxml::xml_node<OJChar_t>        XmlNodeType;
    typedef rapidxml::xml_attribute<OJChar_t>   XmlAttributeType;

private:
    typedef rapidxml::file<OJChar_t>            FileType;

public:
    XmlParser();
    ~XmlParser();

    bool loadFile(const OJString &xmlFileName);

    std::shared_ptr<XmlNodeType> getRootNode(const OJString &tag) const;
    std::shared_ptr<XmlNodeType> getChildNode(const OJString &tag) const;
    std::shared_ptr<XmlNodeType> getNextNode(const OJString &tag) const;

    bool getNodePropertyInt32(std::shared_ptr<XmlNodeType> node,
                            OJInt32_t &num) const;
    bool getNodePropertyString(std::shared_ptr<XmlNodeType> node,
                            OJString &str) const;
    bool getNodeContextInt32(std::shared_ptr<XmlNodeType> node,
                            OJInt32_t &num) const;
    bool getNodeContextString(std::shared_ptr<XmlNodeType> node,
                            OJString &str) const;

private:
   std::shared_ptr<rapidxml::xml_document<OJChar_t> >       doc_;
};





}   // namespace IMUST

#endif  // IMUST_OJ_XML_PARSER_H