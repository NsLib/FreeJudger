#pragma once

#include <memory>
#include "../platformlayer/PlatformLayer.h"

namespace IMUST
{

namespace StringConvert
{

    #define OJ_USE_WIDE_CHAR_SET

    //多字节字符串转换成宽字符字符串
    bool narrowStringToWide(std::wstring & dest, const std::string & src, DWORD code=0);

    //宽字符串转换成多字节字符串
    bool wideStringToNarrow(std::string & dest, const std::wstring & wstr, DWORD code=0);

    JUDGER_API bool OJStringToNarrowString(std::string & dest, const OJString & src);

    JUDGER_API bool NarrowStringToOJString(OJString & dest, const std::string & src);

    JUDGER_API std::string OJStringToNarrowString(const OJString & src);

    JUDGER_API OJString NarrowStringToOJString(const std::string & src);

} //namespace StringConvert


typedef std::shared_ptr<class SqlDriver>    SqlDriverPtr;
typedef std::shared_ptr<class SqlResult>    SqlResultPtr;
typedef std::shared_ptr<class SqlRow>       SqlRowPtr;

class JUDGER_API SqlDriver
{
public:

    virtual bool loadService() = 0;

    virtual void unloadService() = 0;

    virtual bool valid()  const = 0;

    virtual bool connect(const OJString & host,
        OJUInt32_t port,
        const OJString & user,
        const OJString & passwd,
        const OJString & DBName) = 0;

    virtual bool disconect() = 0;

    virtual bool setCharSet(const OJString & charset) = 0;

    virtual OJString escapeString(const OJString & str) = 0;

    virtual bool query(const OJString & sqlString) = 0;

    virtual OJUInt64_t getAffectedRows() = 0;

    virtual OJUInt64_t getInsertID() = 0;

    virtual SqlResultPtr storeResult() = 0;

    virtual OJUInt32_t getErrorCode() = 0;

    virtual OJString getErrorString() = 0;
};

class JUDGER_API SqlResult
{
public:
   
    //行数
    virtual OJUInt64_t getNbRows() const = 0;

    //列数
    virtual OJUInt32_t getNbCols() const = 0;

    //获得标题域
    virtual const OJString getFieldName(OJUInt32_t i) const = 0;

    //域转换成索引
    virtual OJUInt32_t getFieldIndex(const OJString & fieldName) const = 0;

    virtual SqlRowPtr fetchRow() = 0;

};


class JUDGER_API SqlVar
{
public:

    SqlVar(const OJString & v);
    
    bool getBool() const;
    OJInt32_t getInt32() const;
    OJFloat16_t getFloat16() const;
    OJFloat32_t getFloat32() const;
    const OJString & getString() const { return value_; }

private:
    OJString value_;
};


class JUDGER_API SqlRow
{
public:
 
    virtual OJUInt32_t getNbCols() const = 0;

    virtual const SqlVar & getVar(OJUInt32_t index) const = 0;

    virtual const SqlVar & operator[](OJUInt32_t index) const;
};

namespace SqlType
{
    const OJInt32_t MySql = 0;

}//namespace Sql


class JUDGER_API SqlFactory
{
public:
    static SqlDriverPtr createDriver(OJInt32_t type);
};


} // namespace IMUST