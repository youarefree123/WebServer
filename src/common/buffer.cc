// buffer 实现

#include "buffer.h"

Buffer::Buffer(int initBuffSize) : buffer_(initBuffSize), read_pos_(0), write_pos_(0) {}

size_t Buffer::readable_bytes() const {
    return write_pos_ - read_pos_;
}
size_t Buffer::writeable_bytes() const {
    return buffer_.size() - write_pos_;
}

size_t Buffer::prependable_bytes() const {
    return read_pos_;
}

const char* Buffer::peek() const {
    return get_begin_ptr() + read_pos_;
}

void Buffer::retrieve(size_t len) {
    assert(len <= readable_bytes());
    read_pos_ += len;
}

void Buffer::retrieve_until(const char* end) {
    assert(peek() <= end );
    retrieve(end - peek());
}

void Buffer::retrieve_all() {
    bzero(&buffer_[0], buffer_.size());
    read_pos_ = 0;
    write_pos_ = 0;
}

std::string Buffer::retrieve_all_to_str() {
    std::string str(peek(), readable_bytes());
    retrieve_all();
    return str;
}

const char* Buffer::begin_write_const() const {
    return get_begin_ptr() + write_pos_;
}

char* Buffer::begin_write() {
    return get_begin_ptr() + write_pos_;
}

void Buffer::has_written(size_t len) {
    write_pos_ += len;
} 

void Buffer::append(const std::string& str) {
    append(str.data(), str.length());
}

void Buffer::append(const void* data, size_t len) {
    assert(data);
    append(static_cast<const char*>(data), len);
}

void Buffer::append(const char* str, size_t len) {
    assert(str);
    ensure_writeable(len);
    std::copy(str, str + len, begin_write());
    has_written(len);
}

void Buffer::append(const Buffer& buff) {
    append(buff.peek(), buff.readable_bytes());
}

void Buffer::ensure_writeable(size_t len) {
    if(writeable_bytes() < len) {
        make_space(len);
    }
    assert(writeable_bytes() >= len);
}

ssize_t Buffer::read_file(int fd, int* saveErrno) {
    char buff[65535];
    struct iovec iov[2];
    const size_t writable = writeable_bytes();
    /* 分散读， 保证数据全部读完 */
    iov[0].iov_base = get_begin_ptr() + write_pos_;
    iov[0].iov_len = writable;
    iov[1].iov_base = buff;
    iov[1].iov_len = sizeof(buff);

    const ssize_t len = readv(fd, iov, 2);
    if(len < 0) {
        *saveErrno = errno;
    }
    else if(static_cast<size_t>(len) <= writable) {
        write_pos_ += len;
    }
    else {
        write_pos_ = buffer_.size();
        append(buff, len - writable);
    }
    return len;
}

ssize_t Buffer::write_file(int fd, int* saveErrno) {
    size_t readSize = readable_bytes();
    ssize_t len = write(fd, peek(), readSize);
    if(len < 0) {
        *saveErrno = errno;
        return len;
    } 
    read_pos_ += len;
    return len;
}

char* Buffer::get_begin_ptr() {
    return &*buffer_.begin();
}

const char* Buffer::get_begin_ptr() const {
    return &*buffer_.begin();
}

void Buffer::make_space(size_t len) {
    if(writeable_bytes() + prependable_bytes() < len) {
        buffer_.resize(write_pos_ + len + 1);
    } 
    else {
        size_t readable = readable_bytes();
        std::copy(get_begin_ptr() + read_pos_, get_begin_ptr() + write_pos_, get_begin_ptr());
        read_pos_ = 0;
        write_pos_ = read_pos_ + readable;
        assert(readable == readable_bytes());
    }
}




// Buffer::Buffer(int init_size)
// :buffer_(init_size),read_pos_(0),write_pos_(0){}


// size_t Buffer::readable_bytes() const {
//     return write_pos_ - read_pos_;
// }

// size_t Buffer::writeable_bytes() const {
//     return buffer_.size() - write_pos_;
// }

// size_t Buffer::prependable_bytes() const {
//     return read_pos_;
// }

// const char* Buffer::peek() const {
//     return get_begin_ptr() + read_pos_;
// }

// // 这里应该用while的吧
// void Buffer::ensure_writeable(size_t len) {
//     if(writeable_bytes() < len) 
//         make_space(len);
//     assert(writeable_bytes() >= len);
// }

// void Buffer::has_written(size_t len) {
//     write_pos_ += len;
// }

// void Buffer::retrieve(size_t len) {
//     assert(len <= readable_bytes());
//     read_pos_ += len;
// }

// void Buffer::retrieve_until(const char* end) {
//     assert(peek() <= end);
//     retrieve(end - peek());
// }

// void Buffer::retrieve_all() {
//     bzero(&buffer_[0], buffer_.size());
//     read_pos_ = 0;
//     write_pos_ = 0;
// }

// std::string Buffer::retrieve_all_to_str() {
//     std::string str(peek(), readable_bytes());
//     retrieve_all();
//     return str;
// }

// const char* Buffer::begin_write_const() const {
//     return get_begin_ptr() + write_pos_;
// }

// char* Buffer::begin_write(){
//     return get_begin_ptr() + write_pos_;
// }

// void Buffer::append(const std::string& str) {
//     append(str.data(), str.size());
// }

// void Buffer::append(const char* str, size_t len) {
//     assert(static_cast<bool> (str) );
//     ensure_writeable(len);
//     std::copy(str, str+len, begin_write());
//     has_written(len);
// }

// void Buffer::append(const void* data, size_t len) {
//     assert(static_cast<bool> (data));
//     append(static_cast<const char*> (data), len);
// }

// void Buffer::append(const Buffer& buff) {
//     append(buff.peek(),buff.readable_bytes());
// }

// // struct iovec
// // {
// //     void *iov_base;  //指向一个缓冲区，这个缓冲区是存放readv所接收的数据或是writev将要发送的数据
// //     size_t iov_len;  //接收的长度以及实际写入的长度
// // };
// // 分散读readv和集中写writev
// // #include <sys/uio.h>
// // ssize_t readv(int fd, const struct iovec *vector, int count);
// // ssize_t writev(int fd, const struct iovec *vector, int count);
// // 没太搞懂原理，学完apue再来看看

// ssize_t Buffer::read_file(int fd, int* save_errno) {
//     char buff[65535];      // 存储 body 部分的数据
//     struct iovec iov[2];
//     const size_t write_size = writeable_bytes();
//     // 分散读
//     iov[0].iov_base = get_begin_ptr() + write_pos_;
//     iov[0].iov_len = write_size;
//     iov[1].iov_base = buff;
//     iov[1].iov_len = sizeof(buff);

//     const ssize_t len = readv(fd,iov,2);
//     if(len < 0)
//         *save_errno = errno;
//     else if(static_cast<size_t>(len) <= write_size){
//         // 如果没有超过最大可写入大小，那就直接加
//         write_pos_ += len;
//     }
//     else {
//         write_pos_ = buffer_.size();
//         append(buff,len-write_size);
//     }
//     return len;
// }


// ssize_t Buffer::write_file(int fd, int* save_errno) {
//     size_t read_size = readable_bytes();
//     ssize_t len = write(fd,peek(),read_size);
//     if(len < 0) {
//         *save_errno = errno;
//         return len;
//     }
//     read_pos_ += len;
//     return len;
// }

// char* Buffer::get_begin_ptr() {
//     return &(*buffer_.begin());
// }

// const char* Buffer::get_begin_ptr() const {
//     return &(*buffer_.begin());
// }


// void Buffer::make_space(size_t len) {
//     if(writeable_bytes() + prependable_bytes() < len) {
//         buffer_.resize(write_pos_+len+1);
//     }
//     else {
//         // 如果内存够的话就将元素整体往左移
//         size_t read_size = readable_bytes();
//         std::copy(get_begin_ptr()+read_pos_, get_begin_ptr()+write_pos_,get_begin_ptr() );
//         read_pos_ = 0;
//         write_pos_ = read_pos_ + read_size;
//         assert(read_size == readable_bytes());
//     }
// }