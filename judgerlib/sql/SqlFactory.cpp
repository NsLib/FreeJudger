

#include "Sql_mysql.h"

namespace IMUST
{

  
/*static */SqlDriverPtr SqlFactory::createDriver(OJInt32_t type)
{
    if(type == SqlType::MySql)
    {
        return SqlDriverPtr(new MySql::MySqlImpl() );
    }

    assert(false && "wrong sql type!");

    return NULL;
}


}//namespace IMUST