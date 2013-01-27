
#ifndef IMUST_OJ_XML_RAPIDXML_H
#define IMUST_OJ_XML_RAPIDXML_H

#include "Xml.h"

#include "../../thirdpartylib/rapidxml/rapidxml.hpp"
#include "../../thirdpartylib/rapidxml/rapidxml_utils.hpp"
#include "../../thirdpartylib/rapidxml/rapidxml_print.hpp"

namespace IMUST
{

class RapidXmlImpl : public IXml
{
public:

    typedef rapidxml::xml_document<OJChar_t>    XmlDocumentType;
    typedef rapidxml::xml_node<OJChar_t>        XmlNodeType;
    typedef rapidxml::xml_attribute<OJChar_t>   XmlAttributeType;

    typedef std::shared_ptr<XmlDocumentType>    XmlDocumentTypePtr;

    typedef rapidxml::file<OJChar_t>            FileType;
    typedef std::shared_ptr<FileType>           FileTypePtr;

public:
    RapidXmlImpl();

    RapidXmlImpl(XmlNodeType* node, FileTypePtr file, XmlDocumentTypePtr doc);

    ~RapidXmlImpl();

    virtual bool valid() const { return file_ && doc_ && pNode_; }

     virtual const OJChar_t* ctag() const;

    virtual const OJChar_t * cvalue() const;

    virtual OJString tag() const { return ctag(); }

    virtual OJString value() const {  return cvalue(); }

    virtual void setTag(const OJString &tag);

    virtual void setValue(const OJString &value);

public:
    virtual XmlPtr read(const OJString &tag) const ;

    virtual XmlPtr write(const OJString &tag);

    virtual XmlPtr getNextSibling() const;

    virtual bool load(const OJString & fileName);

    virtual bool save(const OJString & fileName) const;

private:
        
    XmlPtr makeNode(XmlNodeType* pNode) const;

    XmlNodeType * findFirstNode(XmlNodeType * pParent, const OJString & tag) const;

    //递归查找第一个结点
    XmlNodeType * findFirstNodeRecursive(XmlNodeType * pParent, const OJString & tag) const;

    XmlNodeType * addFirstNode(XmlNodeType * pParent, const OJString & tag);

    XmlNodeType * addFirstNodeRecursive(XmlNodeType * pParent, const OJString & tag);

private:

    FileTypePtr         file_;
    XmlDocumentTypePtr  doc_;
    XmlNodeType*        pNode_;
};

} // namespace IMUST

#endif//  IMUST_OJ_XML_RAPIDXML_H