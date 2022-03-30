// 阻塞队列,包含阻塞队列包装模板类以及一系列模板函数

#pragma once
#include <mutex>
#include <deque>
#include <condition_variable>
#include <sys/time.h>
#include <cassert>


template<class T>
class BlockDeque {
public:
    explicit BlockDeque(size_t MaxCapacity = 1000);

    ~BlockDeque();

    void clear();

    bool empty();

    bool full();

    void close();

    size_t size();

    size_t capacity();

    T front();

    T back();

    void push_back(const T &item);

    void push_front(const T &item);

    bool pop(T &item);

    bool pop(T &item, int timeout);

    void flush();

private:
    std::deque<T> deq_;

    size_t capacity_;

    std::mutex mtx_;

    bool isClose_;

    std::condition_variable condConsumer_;

    std::condition_variable condProducer_;
};


template<class T>
BlockDeque<T>::BlockDeque(size_t MaxCapacity) :capacity_(MaxCapacity) {
    assert(MaxCapacity > 0);
    isClose_ = false;
}

template<class T>
BlockDeque<T>::~BlockDeque() {
    close();
};

template<class T>
void BlockDeque<T>::close() {
    {   
        std::lock_guard<std::mutex> locker(mtx_);
        deq_.clear();
        isClose_ = true;
    }
    condProducer_.notify_all();
    condConsumer_.notify_all();
};

template<class T>
void BlockDeque<T>::flush() {
    condConsumer_.notify_one();
};

template<class T>
void BlockDeque<T>::clear() {
    std::lock_guard<std::mutex> locker(mtx_);
    deq_.clear();
}

template<class T>
T BlockDeque<T>::front() {
    std::lock_guard<std::mutex> locker(mtx_);
    return deq_.front();
}

template<class T>
T BlockDeque<T>::back() {
    std::lock_guard<std::mutex> locker(mtx_);
    return deq_.back();
}

template<class T>
size_t BlockDeque<T>::size() {
    std::lock_guard<std::mutex> locker(mtx_);
    return deq_.size();
}

template<class T>
size_t BlockDeque<T>::capacity() {
    std::lock_guard<std::mutex> locker(mtx_);
    return capacity_;
}

template<class T>
void BlockDeque<T>::push_back(const T &item) {
    std::unique_lock<std::mutex> locker(mtx_);
    while(deq_.size() >= capacity_) {
        condProducer_.wait(locker);
    }
    deq_.push_back(item);
    condConsumer_.notify_one();
}

template<class T>
void BlockDeque<T>::push_front(const T &item) {
    std::unique_lock<std::mutex> locker(mtx_);
    while(deq_.size() >= capacity_) {
        condProducer_.wait(locker);
    }
    deq_.push_front(item);
    condConsumer_.notify_one();
}

template<class T>
bool BlockDeque<T>::empty() {
    std::lock_guard<std::mutex> locker(mtx_);
    return deq_.empty();
}

template<class T>
bool BlockDeque<T>::full(){
    std::lock_guard<std::mutex> locker(mtx_);
    return deq_.size() >= capacity_;
}

template<class T>
bool BlockDeque<T>::pop(T &item) {
    std::unique_lock<std::mutex> locker(mtx_);
    while(deq_.empty()){
        condConsumer_.wait(locker);
        if(isClose_){
            return false;
        }
    }
    item = deq_.front();
    deq_.pop_front();
    condProducer_.notify_one();
    return true;
}

template<class T>
bool BlockDeque<T>::pop(T &item, int timeout) {
    std::unique_lock<std::mutex> locker(mtx_);
    while(deq_.empty()){
        if(condConsumer_.wait_for(locker, std::chrono::seconds(timeout)) 
                == std::cv_status::timeout){
            return false;
        }
        if(isClose_){
            return false;
        }
    }
    item = deq_.front();
    deq_.pop_front();
    condProducer_.notify_one();
    return true;
}





// template<typename T> 
// class BlockDeque {
// public:
//     explicit BlockDeque(size_t max_capacity = 1000);
//     ~BlockDeque();

//     void clear();      // 清空队列
//     bool empty();      // 判断队列是否为空
//     bool full();       // 判断队列是否为满
//     void close();      // 关闭阻塞队列
//     size_t size();     // 队列中的消息数
//     size_t capacity(); // 队列最大容量

//     T front();                      // 取队首
//     T back();                       // 取队尾
//     void push_back(const T& item); 
//     void push_front(const T& item);
//     bool pop(T& item);              // pop 队首 返回成功与否，删除的变量由item带回
//     bool pop(T& item, int timeout); // 加了定时的pop

//     void flush();                   // notify 一个 消费者线程
// private:
//     std::deque<T> deq_;    // 充当阻塞队列容(双向队列)
//     size_t capacity_;      // 队列最大容量
//     std::mutex mtx_;
//     bool is_close_;        // 用于析构中回收队列
//     std::condition_variable cond_consumer_;    // 条件变量：消费者
//     std::condition_variable cond_producer_;    // 条件变量:生产者
// };



// template<typename T> 
// BlockDeque<T>::BlockDeque(size_t max_capacity)
// : capacity_(max_capacity) {
//     assert(max_capacity > 0);
//     is_close_ = false;
// }

// // 单独设计close是为了给用户一个机会自己回收资源，尽量不在析构函数有异常抛出，参见effetive c++ 条款11
// template<typename T>
// BlockDeque<T>::~BlockDeque() {
//     close(); 
// }

// template<typename T>
// void BlockDeque<T>::close() {
//     {
//         std::lock_guard<std::mutex> locker(mtx_);
//         deq_.clear();
//         is_close_ = true;
//     }
//     cond_consumer_.notify_all();
//     cond_producer_.notify_all();
// }

// template<typename T>
// void BlockDeque<T>::flush() {
//     cond_consumer_.notify_one();
// }

// template<typename T>
// void BlockDeque<T>::clear() {
//     std::lock_guard<std::mutex> locker(mtx_);
//     deq_.clear();
// }

// template<typename T>
// T BlockDeque<T>::front() {
//     std::lock_guard<std::mutex> locker(mtx_);
//     return deq_.front();
// }

// template<typename T>
// T BlockDeque<T>::back() {
//     std::lock_guard<std::mutex> locker(mtx_);
//     return deq_.back();
// }

// template<typename T>
// size_t BlockDeque<T>::size() {
//     std::lock_guard<std::mutex> locker(mtx_);
//     return deq_.size();
// }

// template<typename T>
// size_t BlockDeque<T>::capacity() {
//     std::lock_guard<std::mutex> locker(mtx_);
//     return capacity_;
// }

// template<typename T>
// void BlockDeque<T>::push_back(const T& item) {
//     std::unique_lock<std::mutex> locker(mtx_); // 不直接使用mtx_是因为异常可能引发锁无法释放,需要RAII
//                                                // 不适用lock_guard 是因为pv操作中需要调用lock or unlock
//     cond_producer_.wait(locker, [&]{return deq_.size() < capacity_;} );
//     deq_.push_back(item);
//     cond_consumer_.notify_one();
// }


// template<typename T>
// void BlockDeque<T>::push_front(const T& item) {
//     std::unique_lock<std::mutex> locker(mtx_);
//     cond_producer_.wait(locker, [&]{return deq_.size() < capacity_;} );
//     deq_.push_front(item);
//     cond_consumer_.notify_one();
// }

// template<typename T> 
// bool BlockDeque<T>::empty(){
//     std::lock_guard<std::mutex> locker(mtx_);
//     return deq_.empty();
// }

// template<typename T>
// bool BlockDeque<T>::full(){
//     std::lock_guard<std::mutex> locker(mtx_);
//     return deq_.size() >= capacity_;
// }





// // template<typename T>
// // bool BlockDeque<T>::pop(T& item) {
// //     std::unique_lock<std::mutex> locker(mtx_);
// //     cond_consumer_.wait(locker, [&]{return !deq_.empty();} ); 
// //     if(is_close_) return false; 
// //     item = deq_.front();
// //     deq_.pop_front();
// //     cond_producer_.notify_one();
// //     return true;
// // }
// //  notify_one 每次只会唤醒一个线程，所以可以直接写成上面的样子
// template<typename T>
// bool BlockDeque<T>::pop(T& item) {
//     std::unique_lock<std::mutex> locker(mtx_);
//     while(deq_.empty()) {
//         cond_consumer_.wait(locker);
//         if(is_close_) return false;
//     }
//     item = deq_.front();
//     deq_.pop_front();
//     cond_producer_.notify_one();
//     return true;
// }



// // template <class Rep, class Period>
// //   cv_status wait_for (unique_lock<mutex>& lck,
// //                       const chrono::duration<Rep,Period>& rel_time);

// // template <class Rep, class Period, class Predicate>
// //        bool wait_for (unique_lock<mutex>& lck,
// //                       const chrono::duration<Rep,Period>& rel_time, Predicate pred);
// // template<typename T>
// // bool BlockDeque<T>::pop(T& item, int timeout) {
// //     std::unique_lock<std::mutex> locker(mtx_);
// //     bool check = cond_consumer_.wait_for(
// //         locker,
// //         std::chrono::seconds(timeout),
// //         [&]{ return !deq_.empty(); }
// //     );
// //     if(is_close_ || !check) return false;   // 超时或者 队列关闭，返回false

// //     item = deq_.front();
// //     deq_.pop_front();
// //     cond_producer_.notify_one();
// //     return true;
// // }


// template<class T>
// bool BlockDeque<T>::pop(T &item, int timeout) {
//     std::unique_lock<std::mutex> locker(mtx_);
//     while(deq_.empty()){
//         if(cond_consumer_.wait_for(locker, std::chrono::seconds(timeout)) 
//                 == std::cv_status::timeout){
//             return false;
//         }
//         if(is_close_){
//             return false;
//         }
//     }
//     item = deq_.front();
//     deq_.pop_front();
//     cond_producer_.notify_one();
//     return true;
// }