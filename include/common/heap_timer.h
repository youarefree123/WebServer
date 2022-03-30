# pragma once 

#include <queue>
#include <unordered_map>
#include <time.h>
#include <algorithm>
#include <arpa/inet.h>
#include <functional>
#include <cassert> 
#include <chrono>
#include "log.h"

using TimeoutCallBack = std::function<void()>;      // 回调函数类型
using Clock = std::chrono::high_resolution_clock;         // 高精度时钟
using MS = std::chrono::milliseconds;               // 持续时间
using TimeStamp = Clock::time_point;                // 时间戳


struct TimerNode {
    int id;                   // 定时器编号，唯一指定一个定时器
    TimeStamp expires;        // 期限
    TimeoutCallBack cb;       // 超时回调函数
    bool operator< (const TimerNode& t) {
        return expires < t.expires;
    }
};

class HeapTimer {
public:
    // reserve 避免内存重新分配
    HeapTimer() { heap_.reserve(64); }
    ~HeapTimer() { clear(); }

    void adjust(int id, int new_expires);                     // 调整定时器，任务发生变化时，调整定时器在表中的位置
    void add(int id, int timeout, const TimeoutCallBack& cb); // 添加定时器
    void do_work(int id);                                     // 删除指定id节点，并出发回调函数（无需等待定时器超时）
    void clear();                                             // 手动回收资源
    void tick();                                              // 清除超时节点
    void pop();
    int get_next_tick();                                      // 先tick，然后再得到下一个超时定时器剩余时间

private:
    void del(size_t i);                         // 删除
    void siftup(size_t i);                      // 上滤
    bool siftdown(size_t index, size_t n);      // 下滤
    void swap_node(size_t p, size_t q);         // 交换（更新ref_）
    
    std::vector<TimerNode> heap_;               // 存放定时器
    std::unordered_map<int,size_t> ref_;        // 定时器与heap_中放的位置绑定
};