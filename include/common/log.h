// 单例模式实现日志类

#pragma once

#include <mutex>
#include <string>
#include <thread>
#include <sys/time.h>
#include <string.h>
#include <stdarg.h>           // vastart va_end
#include <assert.h>
#include <sys/stat.h>         //mkdir
#include "block_queue.h"
#include "buffer.h"

class Log {
public:
    void init(
        int level,
        const char* path = "./log",
        const char* suffix = ".log",
        int max_queue_capacity = 1024
    );

    static Log* instance();                // 返回一个日志实例
    static void flush_log_thread();

    void write(int level, const char* format, ...);   //  
    void flush();

    int get_level();      
    void set_level(int level);
    bool is_open() { return is_open_; }

private:
    Log();
    virtual ~Log();
    void append_log_level_title(int level);
    void async_write();                     // 异步读
    

    static const int kLogPathLen = 256;         
    static const int kLogNameLen = 256;     
    static const int kMaxLines = 50000;     // 最大行数
 
    const char* path_;                      // log地址           
    const char* suffix_;                    // 后缀

    int line_count_;                       // 返回当前行数
    int to_day_;                           // 
    bool is_open_;                         // 判断日志是否打开
    Buffer buff_;                          // 缓冲区
    int level_;                            // 日志等级
    bool is_async_;                        // 是否异步

    FILE* fp_;                                              // 操作文件
    std::unique_ptr< BlockDeque<std::string> > deque_ptr_;  // 操作阻塞队列的指针   
    std::unique_ptr< std::thread > write_thread_;           // 操作写线程的指针
    std::mutex mtx_;
};


#define LOG_BASE(level, format, ...) \
    do {\
        Log* log = Log::instance();\
        if (log->is_open() && log->get_level() <= level) {\
            log->write(level, format, ##__VA_ARGS__); \
            log->flush();\
        }\
    } while(0);

#define LOG_DEBUG(format, ...) do {LOG_BASE(0, format, ##__VA_ARGS__)} while(0);
#define LOG_INFO(format, ...) do {LOG_BASE(1, format, ##__VA_ARGS__)} while(0);
#define LOG_WARN(format, ...) do {LOG_BASE(2, format, ##__VA_ARGS__)} while(0);
#define LOG_ERROR(format, ...) do {LOG_BASE(3, format, ##__VA_ARGS__)} while(0);