
#include "Xml_rapidxml.h"

namespace IMUST
{

XmlPtr allocateRapidXml()
{
    return XmlPtr(new RapidXmlImpl());
}

RapidXmlImpl::RapidXmlImpl()
    : doc_(new XmlDocumentType())
    , pNode_(&*doc_)
{
}

RapidXmlImpl::RapidXmlImpl(XmlNodeType* node, FileTypePtr file, XmlDocumentTypePtr doc)
    : pNode_(node)
    , file_(file)
    , doc_(doc)
{
}


RapidXmlImpl::~RapidXmlImpl()
{
}


XmlPtr RapidXmlImpl::read(const OJString &tag) const
{
    XmlNodeType *pChild = findFirstNodeRecursive(pNode_, tag);
    return makeNode(pChild);
}

XmlPtr RapidXmlImpl::write(const OJString &tag)
{
    XmlNodeType *pChild = addFirstNodeRecursive(pNode_, tag);
    return makeNode(pChild);
}

const OJChar_t* RapidXmlImpl::ctag() const
{
    assert(pNode_ && "pNode_ is NULL!");
    return pNode_->name();
}

const OJChar_t * RapidXmlImpl::cvalue() const
{
    assert(pNode_ && "pNode_ is NULL!");
    return pNode_->value();
}


void RapidXmlImpl::setTag(const OJString &tag)
{
    pNode_->name(doc_->allocate_string(tag.c_str()));
}

void RapidXmlImpl::setValue(const OJString &value)
{
    pNode_->value(doc_->allocate_string(value.c_str()));
}


XmlPtr RapidXmlImpl::getNextSibling() const 
{
    XmlNodeType *firstNode = pNode_->next_sibling(pNode_->name());
    if(NULL != firstNode)
    {
        return makeNode(firstNode);
    }
    return NULL;
}

bool RapidXmlImpl::load(const OJString & fileName)
{
    assert(doc_!=NULL && "doc_ is NULL!");

    try
    {
        file_ = FileTypePtr(new FileType(fileName.c_str()));
    }
    catch(std::runtime_error e)
    {
        //e.what();
        return false;
    }

    const int flag = rapidxml::parse_no_data_nodes|rapidxml::parse_trim_whitespace;
    doc_->parse<flag>(file_->data());
    pNode_ = &*doc_;

    return true;
} 

bool RapidXmlImpl::save(const OJString & fileName) const
{
    assert(pNode_ && "pNode_ is NULL!");

    OJOfstream fout(fileName.c_str());
    if(!fout.good())
    {
        return false;
    }

    rapidxml::print(std::ostream_iterator<OJChar_t, OJChar_t>(fout), *pNode_);

    fout.close();
    return true;
}


XmlPtr RapidXmlImpl::makeNode(XmlNodeType* pNode) const
{
    if(NULL != pNode)
    {
        return XmlPtr( new RapidXmlImpl(pNode, file_, doc_) );
    }
    return NULL;
}

RapidXmlImpl::XmlNodeType * RapidXmlImpl::findFirstNode(
    XmlNodeType * pParent, const OJString & tag) const
{
    assert(pParent!=NULL && "pParent is NULL!");

    return pParent->first_node(tag.c_str());
}

//递归查找第一个结点
RapidXmlImpl::XmlNodeType * RapidXmlImpl::findFirstNodeRecursive(
    XmlNodeType * pParent, const OJString & tag) const
{
    if(NULL == pParent)
    {
        return NULL;
    }

    size_t pos = tag.find(OJCh('/'));
    if(pos == tag.npos)
    {
        return findFirstNode(pParent, tag);
    }

    XmlNodeType * pNode = findFirstNode(pParent, tag.substr(0, pos));
    return findFirstNodeRecursive(pNode, tag.substr(pos+1));
}

RapidXmlImpl::XmlNodeType * RapidXmlImpl::addFirstNode(
    XmlNodeType * pParent, const OJString & tag)
{
    XmlNodeType* pNode = findFirstNode(pParent, tag);
    if(NULL == pNode)
    {
        pNode = doc_->allocate_node(rapidxml::node_element);
        pNode->name(doc_->allocate_string(tag.c_str()));
        pParent->append_node(pNode);
    }
    return pNode;
}

RapidXmlImpl::XmlNodeType* RapidXmlImpl::addFirstNodeRecursive(
    XmlNodeType * pParent, const OJString & tag)
{
    if(NULL == pParent)
    {
        return NULL;
    }

    size_t pos = tag.find(OJCh('/'));
    if(pos == tag.npos)
    {
        return addFirstNode(pParent, tag);
    }

    XmlNodeType * pNode = addFirstNode(pParent, tag.substr(0, pos));
    return addFirstNodeRecursive(pNode, tag.substr(pos+1));
}

} //end namespace IMUST