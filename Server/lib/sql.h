#ifndef SQL_H
#define SQL_H
#include    <pthread.h>
#include    <mysql/mysql.h>

struct sqlData{
    pthread_mutex_t   mutex;
    MYSQL             mysql;
};

struct sqlQueryData{
    MYSQL_RES*        result;
    MYSQL_FIELD*      fields;
    MYSQL_ROW*         row;
};

struct sqlData* sql_data_init();
void destroy_sqlData(struct sqlData* sqlData);

struct sqlQueryData* new_queryData();
void destroy_queryData(struct sqlQueryData* queryData);

#endif