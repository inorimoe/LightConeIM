#include "sql.h"
#include "log.h"

struct sqlData* sql_data_init()
{
    struct sqlData *sqlData = new struct sqlData;

    mysql_init(&(sqlData->mysql));
    if(!mysql_real_connect(&(sqlData->mysql), "127.0.0.1", "root", "neverblack", "IM", 0, NULL, 0))
    {
        yolanda_msgx("Failed to connect to database: Error: %s", mysql_error(&sqlData->mysql));
    }
    yolanda_msgx("Successed to connect to database;");
    return sqlData;
}

void destroySqlData(struct sqlData* sqlData)
{
    mysql_close( &sqlData->mysql );
}

struct sqlQueryData* new_queryData()
{
    struct sqlQueryData* querySql = new struct sqlQueryData();
    querySql->result = NULL;
    return querySql;
}

void destroy_queryData(struct sqlQueryData* queryData)
{
    if(queryData->result != NULL){
        mysql_free_result(queryData->result);
    }
    
    delete queryData;
    queryData = NULL;
}