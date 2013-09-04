#pragma once

#include "sql.h"

#include <vector>
#include <mysql.h>


namespace IMUST
{

namespace MySql
{

class MySqlImpl : public SqlDriver
{
public:
    MySqlImpl(void);
    virtual ~MySqlImpl(void);

    virtual bool loadService();

    virtual void unloadService();

    virtual bool valid()  const;

    virtual bool connect(const OJString & host,
        OJUInt32_t port,
        const OJString & user,
        const OJString & passwd,
        const OJString & DBName);

    virtual bool disconect();

    bool setOption(mysql_option option, const OJString & arg);

    virtual bool setCharSet(const OJString & charset);

    virtual OJString escapeString(const OJString & str);

    virtual bool query(const OJString & sqlString);

    virtual OJUInt64_t getAffectedRows();

    virtual OJUInt64_t getInsertID();

    virtual SqlResultPtr storeResult();

    virtual OJUInt32_t getErrorCode();

    virtual OJString getErrorString();

private:
    MYSQL   mysql_ ;
    bool    valid_;
};


class MySqlResultImpl : public SqlResult
{
public:

    explicit MySqlResultImpl(MYSQL_RES * res);

    ~MySqlResultImpl();
   
    //行数
    virtual OJUInt64_t getNbRows() const;

    //列数
    virtual OJUInt32_t getNbCols() const ;

    //获得标题域
    virtual const OJString getFieldName(OJUInt32_t i) const;

    //域转换成索引
    virtual OJUInt32_t getFieldIndex(const OJString & fieldName) const;

    virtual SqlRowPtr fetchRow();

    
private:

    //禁止拷贝构造，复制
    MySqlResultImpl(const MySqlResultImpl & );
    const MySqlResultImpl & operator=(const MySqlResultImpl &);

private:
    MYSQL_RES* pResult_;

};


class MySqlRowImpl : public SqlRow
{
public:
    typedef std::vector<SqlVar> VarVector;

public:

    MySqlRowImpl(MYSQL_ROW rowData, OJUInt32_t numCols);
 
    virtual OJUInt32_t getNbCols() const;

    virtual const SqlVar & getVar(OJUInt32_t index) const;

private:
    VarVector   rowData_;
};


} //namespace MySql

}//namespace IMUST