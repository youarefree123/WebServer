#include "log.h"




using namespace std;

Log::Log() {
    line_count_ = 0;
    is_async_ = false;
    write_thread_ = nullptr;
    deque_ptr_ = nullptr;
    to_day_ = 0;
    fp_ = nullptr;
}

Log::~Log() {
    if(write_thread_ && write_thread_->joinable()) {
        while(!deque_ptr_->empty()) {
            deque_ptr_->flush();
        };
        deque_ptr_->close();
        write_thread_->join();
    }
    if(fp_) {
        lock_guard<mutex> locker(mtx_);
        flush();
        fclose(fp_);
    }
}

int Log::get_level() {
    lock_guard<mutex> locker(mtx_);
    return level_;
}

void Log::set_level(int level) {
    lock_guard<mutex> locker(mtx_);
    level_ = level;
}

void Log::init(int level = 1, const char* path, const char* suffix,
    int maxQueueSize) {
    is_open_ = true;
    level_ = level;
    if(maxQueueSize > 0) {
        is_async_ = true;
        if(!deque_ptr_) {
            unique_ptr<BlockDeque<std::string>> newDeque(new BlockDeque<std::string>);
            deque_ptr_ = move(newDeque);
            
            std::unique_ptr<std::thread> NewThread(new thread(flush_log_thread));
            write_thread_ = move(NewThread);
        }
    } else {
        is_async_ = false;
    }

    line_count_ = 0;

    time_t timer = time(nullptr);
    struct tm *sysTime = localtime(&timer);
    struct tm t = *sysTime;
    path_ = path;
    suffix_ = suffix;
    char fileName[kLogNameLen] = {0};
    snprintf(fileName, kLogNameLen - 1, "%s/%04d_%02d_%02d%s", 
            path_, t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, suffix_);
    to_day_ = t.tm_mday;

    {
        lock_guard<mutex> locker(mtx_);
        buff_.retrieve_all();
        if(fp_) { 
            flush();
            fclose(fp_); 
        }

        fp_ = fopen(fileName, "a");
        if(fp_ == nullptr) {
            mkdir(path_, 0777);
            fp_ = fopen(fileName, "a");
        } 
        assert(fp_ != nullptr);
    }
}

void Log::write(int level, const char *format, ...) {
    struct timeval now = {0, 0};
    gettimeofday(&now, nullptr);
    time_t tSec = now.tv_sec;
    struct tm *sysTime = localtime(&tSec);
    struct tm t = *sysTime;
    va_list vaList;

    /* 日志日期 日志行数 */
    if (to_day_ != t.tm_mday || (line_count_ && (line_count_  %  kMaxLines == 0)))
    {
        unique_lock<mutex> locker(mtx_);
        locker.unlock();
        
        char newFile[kLogNameLen];
        char tail[36] = {0};
        snprintf(tail, 36, "%04d_%02d_%02d", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);

        if (to_day_ != t.tm_mday)
        {
            snprintf(newFile, kLogNameLen - 72, "%s/%s%s", path_, tail, suffix_);
            to_day_ = t.tm_mday;
            line_count_ = 0;
        }
        else {
            snprintf(newFile, kLogNameLen - 72, "%s/%s-%d%s", path_, tail, (line_count_  / kMaxLines), suffix_);
        }
        
        locker.lock();
        flush();
        fclose(fp_);
        fp_ = fopen(newFile, "a");
        assert(fp_ != nullptr);
    }

    {
        unique_lock<mutex> locker(mtx_);
        line_count_++;
        int n = snprintf(buff_.begin_write(), 128, "%d-%02d-%02d %02d:%02d:%02d.%06ld ",
                    t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
                    t.tm_hour, t.tm_min, t.tm_sec, now.tv_usec);
                    
        buff_.has_written(n);
        
        append_log_level_title(level);
        
        va_start(vaList, format);
        int m = vsnprintf(buff_.begin_write(), buff_.writeable_bytes(), format, vaList);
        va_end(vaList);

        buff_.has_written(m);
        buff_.append("\n\0", 2);

        if(is_async_ && deque_ptr_ && !deque_ptr_->full()) {
            deque_ptr_->push_back(buff_.retrieve_all_to_str());
        } else {
            fputs(buff_.peek(), fp_);
        }
        buff_.retrieve_all();
    }
}

void Log::append_log_level_title(int level) {
    switch(level) {
    case 0:
        buff_.append("[debug]: ", 9);
        break;
    case 1:
        buff_.append("[info] : ", 9);
        break;
    case 2:
        buff_.append("[warn] : ", 9);
        break;
    case 3:
        buff_.append("[error]: ", 9);
        break;
    default:
        buff_.append("[info] : ", 9);
        break;
    }
}

void Log::flush() {
    if(is_async_) { 
        deque_ptr_->flush(); 
    }
    fflush(fp_);
}

void Log::async_write() {
    string str = "";
    while(deque_ptr_->pop(str)) {
        lock_guard<mutex> locker(mtx_);
        fputs(str.c_str(), fp_);
    }
}

Log* Log::instance() {
    static Log inst;
    return &inst;
}

void Log::flush_log_thread() {
    Log::instance()->async_write();
}








// Log::Log() {
//     line_count_ = 0,
//     is_async_ = false;
//     write_thread_ = nullptr;
//     deque_ptr_ptr_ = nullptr;
//     to_day_  = 0; 
//     fp_ = nullptr;
// }
// // 关闭文件和等待写线程
// Log::~Log() {
//     if(write_thread_ && write_thread_->joinable() ) {
//         while(!deque_ptr_ptr_->empty()) {
//             deque_ptr_ptr_->flush();
//         }
//         deque_ptr_ptr_->close();
//         write_thread_->join();
//     }
//     if(fp_) {
//         std::lock_guard<std::mutex> locker(mtx_);
//         flush();
//         fclose(fp_);
//     }
// }


// int Log::get_level() {
//     std::lock_guard<std::mutex> locker(mtx_);
//     return level_;
// }

// void Log::set_level(int level) {
//     std::lock_guard<std::mutex> locker(mtx_);
//     level_ = level;
// }




// void Log::init(int level = 1, const char* path, const char* suffix, int max_queue_size) {
//     is_open_ = true;
//     level_ = level;
    
//     if(max_queue_size > 0) {
//         is_async_ = true;
//         if(!deque_ptr_ptr_) {
//             deque_ptr_ptr_ =  std::make_unique< BlockDeque<std::string> >();
//             write_thread_ = std::make_unique< std::thread > (flush_log_thead);
//             // std::unique_ptr< BlockDeque<std::string> > newDeque(new BlockDeque<std::string>);
//             // deque_ptr_ptr_ = move(newDeque);
            
//             // std::unique_ptr<std::thread> NewThread(new std::thread(flush_log_thead));
//             // write_thread_ = move(NewThread);
//         }
//     }
//     else 
//         is_async_ = false;
    
//     line_count_ = 0;
//     time_t timer = time(nullptr);
//     struct tm* sys_tmie = localtime(&timer);
//     struct tm t = *sys_tmie;
//     path_ = path;
//     suffix_ = suffix;
//     char file_name[kLogNameLen] = {0};
   
//     snprintf(file_name, kLogNameLen - 1, "%s/%04d_%02d_%02d%s", 
//             path_, t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, suffix_);
//     to_day_ = t.tm_mday;

//     {
//         std::lock_guard<std::mutex> locker(mtx_);
        
//         buff_.retrieve_all();
         
//         if(fp_){
//             flush();
//             fclose(fp_);
//         }
//         fp_ = fopen(file_name,"a");
//         if(fp_ == nullptr) {
//             mkdir(path_,0777);
//             fp_ = fopen(file_name, "a");
//         } 
//         assert(fp_ != nullptr);
//     }
// }


// void Log::write(int level, const char* format, ...) {
//     struct timeval now = {0,0};
//     gettimeofday(&now,nullptr);
//     time_t t_sec = now.tv_sec;
//     struct tm* sys_time = localtime(&t_sec);
//     struct tm  t = *sys_time;
//     va_list vaList;

//     if(to_day_ != t.tm_mday || (line_count_ && (line_count_ % kMaxLines == 0)) ) {
//         std::unique_lock<std::mutex> locker(mtx_);
//         locker.unlock();
        
//         char new_file[kLogNameLen];
//         char tail[36] = {0};
//         snprintf(tail, 36, "%04d_%02d_%02d", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);

//         if (to_day_ != t.tm_mday)
//         {
//             snprintf(new_file, kLogNameLen - 72, "%s/%s%s", path_, tail, suffix_);
//             to_day_ = t.tm_mday;
//             line_count_ = 0;
//         }
//         else {
//             snprintf(new_file, kLogNameLen - 72, "%s/%s-%d%s", path_, tail, (line_count_  / kMaxLines), suffix_);
//         }
        
//         locker.lock();
//         flush();
//         fclose(fp_);
//         fp_ = fopen(new_file, "a");
//         assert(fp_ != nullptr);
//     }


//     {
//         std::unique_lock<std::mutex> locker(mtx_);
//         line_count_++;
//         int n = snprintf(buff_.begin_write(), 128, "%d-%02d-%02d %02d:%02d:%02d.%06ld ",
//                     t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
//                     t.tm_hour, t.tm_min, t.tm_sec, now.tv_usec);
                    
//         buff_.has_written(n);
//         append_log_level_title(level);

//         va_start(vaList, format);
//         int m = vsnprintf(buff_.begin_write(), buff_.writeable_bytes(), format, vaList);
//         va_end(vaList);

//         buff_.has_written(m);
//         buff_.append("\n\0", 2);
    
        
        
//         if(is_async_ && deque_ptr_ptr_ && !deque_ptr_ptr_->full()) {
//             deque_ptr_ptr_->push_back(buff_.retrieve_all_to_str());
//         } else {
            
//             fputs(buff_.peek(), fp_);
//         }
        
//         buff_.retrieve_all();
//     }
// }

// void Log::append_log_level_title(int level) {
//     switch(level) {
//     case 0:
//         buff_.append("[debug]: ", 9);
//         break;
//     case 1:
//         buff_.append("[info] : ", 9);
//         break;
//     case 2:
//         buff_.append("[warn] : ", 9);
//         break;
//     case 3:
//         buff_.append("[error]: ", 9);
//         break;
//     default:
//         buff_.append("[info] : ", 9);
//         break;
//     }
// }



// void Log::async_write() {
//    std::string str = "";
//     while(deque_ptr_ptr_->pop(str)) {
//         std::lock_guard<std::mutex> locker(mtx_);
//         fputs(str.c_str(), fp_);
//     }
// }

// void Log::flush() {
//     if(is_async_) { 
//         deque_ptr_ptr_->flush(); 
//     }
//     fflush(fp_);
// }

// Log* Log::instance() {
//     static Log inst;                // c++ 11 支持static 成员类内初始化
//     return &inst;
// }

// void Log::flush_log_thead() {
//     Log::instance()->async_write();
// }