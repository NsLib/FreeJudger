
#ifndef IMUST_OJ_XML_H
#define	IMUST_OJ_XML_H

#include <memory>
#include <vector>
#include "../platformlayer/PlatformLayer.h"

namespace IMUST
{

typedef std::shared_ptr<class IXml> XmlPtr;
typedef std::vector<XmlPtr> XmlPtrVector;

class JUDGER_API IXml
{
public:
    IXml();
    virtual ~IXml();

    virtual bool valid() const = 0;

    virtual OJString tag() const = 0;
    virtual const OJChar_t* ctag() const = 0;
    virtual void setTag(const OJString &tag) = 0;

    virtual OJString value() const = 0;
    virtual const OJChar_t * cvalue() const = 0;
    virtual void setValue(const OJString &tag) = 0;

    void getInt32(OJInt32_t &d) const;
    void getFloat16(OJFloat16_t &d) const;
    void getFloat32(OJFloat32_t &d) const;
    void getBool(bool &d) const;

    void setInt32(OJInt32_t d);
    void setFloat16(OJFloat16_t d);
    void setFloat32(OJFloat32_t d);
    void setBool(bool d);

    bool readInt32(const OJString &tag, OJInt32_t &d) const;
    bool readFloat16(const OJString &tag, OJFloat16_t &d) const;
    bool readFloat32(const OJString &tag, OJFloat32_t &d) const;
    bool readString(const OJString &tag, OJString &d) const;
    bool readBool(const OJString &tag, bool &d) const;

    void writeInt32(const OJString &tag, OJInt32_t d);
    void writeFloat16(const OJString &tag, OJFloat16_t d);
    void writeFloat32(const OJString &tag, OJFloat32_t d);
    void writeString(const OJString &tag, const OJString & d);
    void wirteBool(const OJString &tag, bool d);

public:
    virtual XmlPtr read(const OJString &tag) const = 0;
    virtual XmlPtr write(const OJString &tag) = 0;
    virtual XmlPtr getNextSibling() const = 0;
    bool reads(const OJString & tag, XmlPtrVector & vector);
    virtual bool load(const OJString & fileName) = 0;
    virtual bool save(const OJString & fileName) const = 0;
};

class JUDGER_API XmlFactory
{
public:
    static XmlPtr getXml(const OJString &xmlType);

private:
    XmlFactory();
    ~XmlFactory();

};

}   // namespace IMUST

#endif  // IMUST_OJ_XML_H