#pragma once 

#include <mysql/mysql.h>      // usr/include/mysql
#include <string>
#include <queue>
#include <mutex>
#include <semaphore.h>        // 信c号量
#include <condition_variable>    // 改用条件变量
#include <thread>
#include "log.h"

// 单例模式实现数据库连接池类
class SqlConnPool {
public:
    static SqlConnPool* instance(); // 得到实例
    MYSQL* get_conn();              // 得到数据库连接,如果全都在处理业务，返回空指针并log WARN
    void free_conn(MYSQL* conn);    // 释放数据库连接
    int get_free_conn_count();      // get空闲连接数
    void init(const char* host, int port,
              const char* user, const char* pwd,
              const char* db_name, int conn_size);          // 设置最大连接数，初始化信号量，将连接数据库的指针存入队列
    void close_pool();                                      // 关闭连接池

private:
    SqlConnPool();
    ~SqlConnPool();

    int kMaxConn_;       // 最大连接数，只作用于初始化信号量
    // int use_count_;        弃用
    // int free_count_;

    std::queue<MYSQL*> conn_que_;    // 已与数据库连接,等待业务处理的队列
    std::mutex mtx_;
    sem_t sem_id_;                   // 信号量id
};