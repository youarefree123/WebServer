
    // 线程池只有一个，所以静止拷贝构造与拷贝赋值，默认开8个线程


#pragma once

#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>
#include <cassert>
#include <functional>





class ThreadPool {
public:
    explicit ThreadPool(size_t threadCount = 8): pool_(std::make_shared<Pool>()) {
            assert(threadCount > 0);
            for(size_t i = 0; i < threadCount; i++) {
                std::thread([pool = pool_] {
                    std::unique_lock<std::mutex> locker(pool->mtx);
                    while(true) {
                        if(!pool->tasks.empty()) {
                            auto task = std::move(pool->tasks.front());
                            pool->tasks.pop();
                            locker.unlock();
                            task();
                            locker.lock();
                        } 
                        else if(pool->is_closed) break;
                        else pool->cond.wait(locker);
                    }
                }).detach();
            }
    }

    ThreadPool() = default;

    ThreadPool(ThreadPool&&) = default;
    
    ~ThreadPool() {
        if(static_cast<bool>(pool_)) {
            {
                std::lock_guard<std::mutex> locker(pool_->mtx);
                pool_->is_closed = true;
            }
            pool_->cond.notify_all();
        }
    }

    template<class F>
    void add_task(F&& task) {
        {
            std::lock_guard<std::mutex> locker(pool_->mtx);
            pool_->tasks.emplace(std::forward<F>(task));
        }
        pool_->cond.notify_one();
    }

private:
    struct Pool {
        std::mutex mtx;
        std::condition_variable cond;
        bool is_closed;
        std::queue<std::function<void()>> tasks;
    };
    std::shared_ptr<Pool> pool_;
};








// //  成员变量，线程池和操作它的指针
// //  遵从RAII，析构时也同时关闭所有相关线程
// class ThreadPool {
// public:
//     ThreadPool() = default;
//     ThreadPool(ThreadPool&&) = delete;
//     // explicit ThreadPool(size_t);
//     // ~ThreadPool();

//     ThreadPool& operator=(ThreadPool&&) = delete;

//     // template<typename Func>
//     // void add_task(Func&&);


//     // 线程从任务队列中取出任务，并处理,其中队列是临界资源，需要锁

// //  开启线程处理任务
// explicit ThreadPool(size_t thread_count = 8)
// :pool_ptr_(std::make_shared<Pool>()) 
// {
//     assert(thread_count > 0);
//     for(size_t i = 0; i < thread_count; ++i)
//         std::thread([pool_ptr = pool_ptr_]{
//             std::unique_lock<std::mutex> locker(pool_ptr->mtx);
//             while(true) {
//                 if(pool_ptr->is_closed) break;
//                 pool_ptr->cond.wait(locker, [&]{ return !(pool_ptr->tasks.empty()); });
//                 std::function<void()> task = std::move(pool_ptr->tasks.front());
//                 pool_ptr->tasks.pop();
                
//                 locker.unlock();
//                 task();
//                 locker.lock();
//             }
//         }).detach();
//         // std::thread( std::bind(process_task,pool_ptr_) ).detach();
// }

// // 关闭链接，唤醒所有线程
// ~ThreadPool() {
//     if(static_cast<bool>(pool_ptr_)) {
//         {
//             std::lock_guard<std::mutex> locker(pool_ptr_->mtx);
//             pool_ptr_->is_closed = true;
//         }
//         pool_ptr_->cond.notify_all();
//     }
// }

// // 添加任务（通过完美转发）并唤醒wait的线程
// template<typename Func>
// void add_task(Func&& task) {
//     {
//         std::lock_guard<std::mutex> locker(pool_ptr_->mtx);
//         pool_ptr_->tasks.emplace(std::forward<Func>(task));
//     }
//     pool_ptr_->cond.notify_one();          
// }


// private:
//     struct Pool {
//         std::mutex mtx;
//         bool is_closed; 
//         std::condition_variable cond; 
//         std::queue< std::function<void()> > tasks;    
//     };
//     std::shared_ptr<Pool> pool_ptr_; 
// };

