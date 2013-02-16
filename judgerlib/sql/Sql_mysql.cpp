#include "Sql_mysql.h"


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

    std::string ojHost = StringConvert::OJStringToNarrowString(host);
    std::string ojUser = StringConvert::OJStringToNarrowString(user);
    std::string ojPassword = StringConvert::OJStringToNarrowString(password);
    std::string ojDBName = StringConvert::OJStringToNarrowString(DBName);

    valid_ = (NULL != mysql_real_connect(&mysql_, 
        ojHost.c_str(), 
        ojUser.c_str(), 
        ojPassword.c_str(), 
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
    std::string ojArg = StringConvert::OJStringToNarrowString(arg);

    return 0 == mysql_options(&mysql_, option, ojArg.c_str());
}

bool MySqlImpl::setCharSet(const OJString & charset)
{
    return setOption(MYSQL_SET_CHARSET_NAME, charset.c_str());
}

OJString MySqlImpl::escapeString(const OJString & str)
{
    if(str.empty())
    {
        return str;
    }

    std::string ansicStr = StringConvert::OJStringToNarrowString(str);
    
    std::string destStr(ansicStr.length()*2, '\0');
    size_t pos = mysql_real_escape_string(&mysql_, &destStr[0], ansicStr.c_str(), ansicStr.length());
    destStr.erase(pos);

    return StringConvert::NarrowStringToOJString(destStr);
}

bool MySqlImpl::query(const OJString & sqlString)
{
    std::string sql = StringConvert::OJStringToNarrowString(sqlString);

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
    return StringConvert::NarrowStringToOJString(mysql_error(&mysql_));
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
const OJString MySqlResultImpl::getFieldName(OJUInt32_t i) const
{
    assert(i>=0 && i<getNbCols() && "getFieldName out of range");

    MYSQL_FIELD* pField = mysql_fetch_field_direct(pResult_, i);
    std::string tempStr(pField->name, pField->name_length);

    return StringConvert::NarrowStringToOJString(tempStr);
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
            StringConvert::NarrowStringToOJString(tempStr, rowData[i]);
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