#pragma once 
#include "sql_conn_pool.h"
// RAII 的方式拿到 数据库连接
class SqlConnRAII {
public:
    SqlConnRAII(MYSQL** sql, SqlConnPool* conn_pool) {
        assert(conn_pool);
        *sql = conn_pool->get_conn();
        sql_ = *sql;
        conn_pool_ = conn_pool;
    }

    ~SqlConnRAII() {
        if(sql_) conn_pool_->free_conn(sql_);
    }
private:
    MYSQL* sql_;
    SqlConnPool* conn_pool_;
};