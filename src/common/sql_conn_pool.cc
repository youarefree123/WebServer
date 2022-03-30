#include "sql_conn_pool.h"

SqlConnPool::SqlConnPool(){}

SqlConnPool::~SqlConnPool() {
    close_pool();
}

SqlConnPool* SqlConnPool::instance() {
    static SqlConnPool conn_pool_;
    return &conn_pool_;
}



void SqlConnPool::init(
    const char* host,
    int port,
    const char* user,
    const char* pwd,
    const char* db_name,
    int conn_size = 10
) {
    assert(conn_size > 0);
    for(int i = 0; i < conn_size; ++i) {
        MYSQL* sql = nullptr;
        sql = mysql_init(sql);
        if(!sql) LOG_ERROR("MySql init error!");
        
        sql = mysql_real_connect(sql, host, user, pwd, db_name, port, nullptr, 0);
        if(!sql) LOG_ERROR("MySql connect error!");

        conn_que_.push(sql);
    }
    kMaxConn_ = conn_size;
    sem_init(&sem_id_, 0, kMaxConn_);      // 初始化信号量， 0 表示线程间同步， kMaxConn_ 代表初始资源数
}

MYSQL* SqlConnPool::get_conn() {
    MYSQL *sql = nullptr;
    if(conn_que_.empty()) {
        LOG_WARN("SqlConnPool busy!");
        return nullptr;
    }

    sem_wait(&sem_id_);              // p 操作
    {
        std::lock_guard<std::mutex> locker(mtx_);
        sql = conn_que_.front();
        conn_que_.pop();
    }
    return sql;
}


void SqlConnPool::free_conn(MYSQL* sql) {
    if(!sql){
        LOG_ERROR("Free null sql!");
        assert(sql);
    }
    std::lock_guard<std::mutex> locker(mtx_);
    conn_que_.push(sql);
    sem_post(&sem_id_);  // v 操作
}

void SqlConnPool::close_pool() {
    std::lock_guard<std::mutex> locker(mtx_);
    while( !conn_que_.empty() ) {
        MYSQL* item = conn_que_.front();
        conn_que_.pop();
        mysql_close(item);
    }
    mysql_library_end();   // 终止使用MySQL库
}

int SqlConnPool::get_free_conn_count() {
    std::lock_guard<std::mutex> locker(mtx_);
    return conn_que_.size();
}

