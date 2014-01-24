#include "Sql_mysql.h"

#include "../util/StringTool.h"


namespace IMUST
{

namespace MySql
{

MySqlImpl::MySqlImpl(void)
    : valid_(false)
{
    memset(&mysql_, 0, sizeof(mysql_));
}


MySqlImpl::~MySqlImpl(void)
{
    if(valid_)
    {
        disconect();
    }
}


bool MySqlImpl::loadService()
{
    return 0 == mysql_library_init(0, NULL, NULL);
}

void MySqlImpl::unloadService()
{
    mysql_library_end();
}

bool MySqlImpl::valid()  const
{
    return valid_;
}

bool MySqlImpl::connect(const OJString & host,
    OJUInt32_t port,
    const OJString & user,
    const OJString & password,
    const OJString & DBName)
{
    if(valid_)
    {
        return false;
    }

    if(!mysql_init(&mysql_))
    {
        return false;
    }

    std::string ojHost, ojUser, ojPwd, ojDBName;
    
    OJString2UTF8(ojHost, host);
    OJString2UTF8(ojUser, user);
    OJString2UTF8(ojPwd, password);
    OJString2UTF8(ojDBName, DBName);


    valid_ = (NULL != mysql_real_connect(&mysql_, 
        ojHost.c_str(), 
        ojUser.c_str(), 
        ojPwd.c_str(), 
        ojDBName.c_str(),
        port,
        NULL, 0));
    
    return valid_ ;
}

bool MySqlImpl::disconect()
{
    if(!valid_)
    {
        return false;
    }

    mysql_close(&mysql_);
    valid_ = false;

    return true;
}

bool MySqlImpl::setOption(mysql_option option, const OJString & arg)
{
    std::string ojArg;
    OJString2UTF8(ojArg, arg);

    return 0 == mysql_options(&mysql_, option, ojArg.c_str());
}

bool MySqlImpl::setCharSet(const OJString & charset)
{
    return setOption(MYSQL_SET_CHARSET_NAME, charset.c_str());
}

//TODO: 优化此处的算法，去掉mysql_real_escape_string的调用。
void MySqlImpl::escapeString(OJString & str)
{
    if(str.empty()) return;

    std::string utf8Str;
    OJString2UTF8(utf8Str, str);


    std::string destStr(utf8Str.length()*2, '\0');
    size_t pos = mysql_real_escape_string(&mysql_, &destStr[0], utf8Str.c_str(), utf8Str.length());
    destStr.erase(pos);

    UTF82OJString(str, destStr);
}

bool MySqlImpl::query(const OJString & sqlString)
{
    std::string sql;
    OJString2UTF8(sql, sqlString);

    return 0 == mysql_real_query(&mysql_, sql.c_str(), sql.length());
}

OJUInt64_t MySqlImpl::getAffectedRows()
{
    return mysql_affected_rows(&mysql_);
}

OJUInt64_t MySqlImpl::getInsertID()
{
    return mysql_insert_id(&mysql_);
}

SqlResultPtr MySqlImpl::storeResult()
{
    MYSQL_RES* result = mysql_store_result(&mysql_);
    if (NULL == result)
    {
        return NULL;
    }
    
    return SqlResultPtr(new MySqlResultImpl(result));
}

OJUInt32_t MySqlImpl::getErrorCode()
{
    return mysql_errno(&mysql_);
}

OJString MySqlImpl::getErrorString()
{
    OJString result;
    UTF82OJString(result, mysql_error(&mysql_));
    return result;
}



//////////////////////////////////////////////////
MySqlResultImpl::MySqlResultImpl(MYSQL_RES * res)
    : pResult_(res)
{
}

MySqlResultImpl::~MySqlResultImpl()
{
    if (pResult_)
    {
        mysql_free_result(pResult_);
    }
}

//行数
OJUInt64_t MySqlResultImpl::getNbRows() const
{
    return mysql_num_rows(pResult_);
}

//列数
OJUInt32_t MySqlResultImpl::getNbCols() const 
{
    return mysql_num_fields(pResult_);
}

//获得标题域
OJString MySqlResultImpl::getFieldName(OJUInt32_t i) const
{
    assert(i>=0 && i<getNbCols() && "getFieldName out of range");

    MYSQL_FIELD* pField = mysql_fetch_field_direct(pResult_, i);
    std::string tempStr(pField->name, pField->name_length);

    OJString result;
    UTF82OJString(result, tempStr);
    return result;
}

//域转换成索引
OJUInt32_t MySqlResultImpl::getFieldIndex(const OJString & fieldName) const
{
    OJUInt32_t cols = getNbCols();
    for (OJUInt32_t i=0; i<cols; ++i)
    {
        if (fieldName == getFieldName(i))
        {
            return i;
        }
    }
    return -1;
}

SqlRowPtr MySqlResultImpl::fetchRow()
{
    MYSQL_ROW rowData = mysql_fetch_row(pResult_);
    if (NULL == rowData)
    {
        return NULL;
    }

    return SqlRowPtr(new MySqlRowImpl(rowData, getNbCols()));
}

//////////////////////////////////////////////////

MySqlRowImpl::MySqlRowImpl(MYSQL_ROW rowData, OJUInt32_t numCols)
{
    rowData_.reserve(numCols);

    OJString tempStr;
    for (OJUInt32_t i=0; i<numCols; ++i)
    {
        if(rowData[i])
        {
            UTF82OJString(tempStr, rowData[i]);
            rowData_.push_back(SqlVar(tempStr));
        }
        else
        {
            rowData_.push_back(SqlVar(OJStr("")));
        }
    }
}
 
OJUInt32_t MySqlRowImpl::getNbCols() const
{
    return rowData_.size();
}

const SqlVar & MySqlRowImpl::getVar(OJUInt32_t index) const
{
    return rowData_[index];
}


} //namespace MySql

} //namespace IMUST