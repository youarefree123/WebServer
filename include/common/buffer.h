// 缓冲区类，包含buffer实体及其相关函数

#pragma once 
#include <cstring>     // perror
#include <iostream>
#include <unistd.h>    // write
#include <sys/uio.h>   // readv
#include <vector> 
#include <atomic>      // 引入c++11 原子操作和类型
#include <cassert>


// 封装好的缓冲区类，包含缓冲区，读写位置和相关的各种操作
class Buffer{
public:
    Buffer(int init_size = 1024);
    ~Buffer() = default;

    // 求缓冲区三个部分的大小 prepend 块预留一小部分空间方便以后添加消息长度，Muduo 特有
    size_t writeable_bytes() const;
    size_t readable_bytes() const;
    size_t prependable_bytes() const;

    // peek() 得到读位置开始位置的指针 == begin_read ？ 
    // ensure_writeable 确保可读，容量不够的话buffer扩容
    // has_written 将write_pos_往后加len位，表示成功读取了len长度的消息
    const char* peek() const;
    void ensure_writeable(size_t len);
    void has_written(size_t len);

    // 根据参数取出不同size的buffer(只更新了read_pos的位置)
    void retrieve(size_t len);
    void retrieve_until(const char* end);
    void retrieve_all();
    std::string retrieve_all_to_str();

    // 得到写开始位置的指针,const版本用于http解析中
    const char* begin_write_const() const;
    char* begin_write();

    // 追加数据
    void append(const std::string& str);
    void append(const char* str, size_t len);
    void append(const void* data, size_t len);
    void append(const Buffer& buff);

    // 分散读readv和集中写writev
    // ssize_t 有符号整型, size_t 无符号整型
    ssize_t read_file(int fd, int* save_errno);
    ssize_t write_file(int fd, int* save_errno);

private:
    char* get_begin_ptr();                  // 得到buffer开端的指针。使用原始指针方便操作
    const char* get_begin_ptr() const;      // 上面的重载，为啥要这样干？buffer会被修改吗？         
    void make_space(size_t len);           // buffer扩容 

    std::vector<char> buffer_;              // 用于存放报文
    std::atomic<std::size_t> read_pos_;     // 当前读到的位置,原子变量
    std::atomic<std::size_t> write_pos_;    // 当前写的位置，原子变量
};