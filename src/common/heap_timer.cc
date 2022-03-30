// 自己写的版本有问题，后续排查
#include "heap_timer.h"

void HeapTimer::siftup(size_t i) {
    assert(i >= 0 && i < heap_.size());
    size_t j = (i - 1) / 2;
    while(j >= 0) {
        if(heap_[j] < heap_[i]) { break; }
        swap_node(i, j);
        i = j;
        j = (i - 1) / 2;
    }
}

void HeapTimer::swap_node(size_t i, size_t j) {
    assert(i >= 0 && i < heap_.size());
    assert(j >= 0 && j < heap_.size());
    std::swap(heap_[i], heap_[j]);
    ref_[heap_[i].id] = i;
    ref_[heap_[j].id] = j;
} 

bool HeapTimer::siftdown(size_t index, size_t n) {
    assert(index >= 0 && index < heap_.size());
    assert(n >= 0 && n <= heap_.size());
    size_t i = index;
    size_t j = i * 2 + 1;
    while(j < n) {
        if(j + 1 < n && heap_[j + 1] < heap_[j]) j++;
        if(heap_[i] < heap_[j]) break;
        swap_node(i, j);
        i = j;
        j = i * 2 + 1;
    }
    return i > index;
}

void HeapTimer::add(int id, int timeout, const TimeoutCallBack& cb) {
    assert(id >= 0);
    size_t i;
    if(ref_.count(id) == 0) {
        /* 新节点：堆尾插入，调整堆 */
        i = heap_.size();
        ref_[id] = i;
        heap_.push_back({id, Clock::now() + MS(timeout), cb});
        siftup(i);
    } 
    else {
        /* 已有结点：调整堆 */
        i = ref_[id];
        heap_[i].expires = Clock::now() + MS(timeout);
        heap_[i].cb = cb;
        if(!siftdown(i, heap_.size())) {
            siftup(i);
        }
    }
}

void HeapTimer::do_work(int id) {
    /* 删除指定id结点，并触发回调函数 */
    if(heap_.empty() || ref_.count(id) == 0) {
        return;
    }
    size_t i = ref_[id];
    TimerNode node = heap_[i];
    node.cb();
    del(i);
}

void HeapTimer::del(size_t index) {
    /* 删除指定位置的结点 */
    assert(!heap_.empty() && index >= 0 && index < heap_.size());
    /* 将要删除的结点换到队尾，然后调整堆 */
    size_t i = index;
    size_t n = heap_.size() - 1;
    assert(i <= n);
    if(i < n) {
        swap_node(i, n);
        if(!siftdown(i, n)) {
            siftup(i);
        }
    }
    /* 队尾元素删除 */
    ref_.erase(heap_.back().id);
    heap_.pop_back();
}

void HeapTimer::adjust(int id, int timeout) {
    /* 调整指定id的结点 */
    assert(!heap_.empty() && ref_.count(id) > 0);
    heap_[ref_[id]].expires = Clock::now() + MS(timeout);;
    siftdown(ref_[id], heap_.size());
}

void HeapTimer::tick() {
    /* 清除超时结点 */
    if(heap_.empty()) {
        return;
    }
    while(!heap_.empty()) {
        TimerNode node = heap_.front();
        if(std::chrono::duration_cast<MS>(node.expires - Clock::now()).count() > 0) { 
            break; 
        }
        node.cb();
        pop();
    }
}

void HeapTimer::pop() {
    assert(!heap_.empty());
    del(0);
}

void HeapTimer::clear() {
    ref_.clear();
    heap_.clear();
}

int HeapTimer::get_next_tick() {
    tick();
    size_t res = -1;
    if(!heap_.empty()) {
        res = std::chrono::duration_cast<MS>(heap_.front().expires - Clock::now()).count();
        if(res < 0) { res = 0; }
    }
    return res;
}







// void HeapTimer::siftup(size_t i) {
//     assert(i >= 0 && i < heap_.size());
//     size_t j = (i-1)/2;
//     while(j >= 0) {
//         if(heap_[j] < heap_[i]) break;
//         swap_node(i,j);
//         i = j;
//         j = (i-1)/2;
//     }
// }

// bool HeapTimer::siftdown(size_t index, size_t n) {
//     assert(index >= 0 && index < heap_.size());
//     assert(n >= 0 && n <= heap_.size());
//     size_t i = index;
//     size_t j = i*2 + 1;
//     while(j < n) {
//         if(j + 1 < n && heap_[j+1] < heap_[j]) ++j;
//         if(heap_[i] < heap_[j]) break;
//         swap_node(i,j);
//         j = i*2+1;
//     }
//     return i > index;
// }


// void HeapTimer::swap_node(size_t i, size_t j) {
//     assert(i >= 0 && i < heap_.size());
//     assert(j >= 0 && j < heap_.size());
//     std::swap(heap_[i], heap_[j]);
//     ref_[heap_[i].id] = i;
//     ref_[heap_[j].id] = j;
// }

// void HeapTimer::add(int id, int timeout, const TimeoutCallBack& cb) {
//     assert(id >= 0);
//     size_t i;
//     if(ref_.count(id) == 0) {
//         // 插入新定时器节点
//         i = heap_.size();
//         ref_[id] = i;
//         heap_.push_back( {id,Clock::now()+MS(timeout),cb } );
//         siftup(i);
//     }
//     else {
//         // 存在改节点，重新设置超时时间和回调函数
//         i = ref_[id];
//         heap_[i].expires = Clock::now() + MS(timeout);
//         heap_[i].cb = cb;
//         // 这操作我没看懂，后面细看
//         if( !siftdown(i,heap_.size()) ) {
//             siftup(i);
//         }
//     }
// }


// void HeapTimer::do_work(int id) {
//     if(heap_.empty() || ref_.count(id) == 0) return;
//     size_t i  = ref_[id];
//     TimerNode node = heap_[i];
//     node.cb();
//     del(i);
// }

// void HeapTimer::del(size_t index) {
//     assert(!heap_.empty() && index >= 0 && index < heap_.size());
//     // 将要删除的节点换到队尾，然后跳整堆
//     size_t i = index;
//     size_t n = heap_.size()-1;
//     assert( i <= n );
//     if(i < n) {
//         swap_node(i,n);
//         if(!siftdown(i,n))
//             siftup(i);
//     }
//     ref_.erase(heap_.back().id);
//     heap_.pop_back();
// }

// void HeapTimer::adjust(int id, int timeout) {
//     assert(!heap_.empty() && ref_.count(id) > 0);
//     heap_[ref_[id]].expires = Clock::now() + MS(timeout);
//     siftdown(ref_[id],heap_.size());
// }

// void HeapTimer::tick() {
//     if(heap_.empty()) return;
//     while(!heap_.empty()) {
//         TimerNode node = heap_.front();
//         if(std::chrono::duration_cast<MS>(node.expires - Clock::now()).count() > 0) { 
//             break; 
//         }
//         node.cb();
//         pop();
//     }
// }

// void HeapTimer::pop() {
//     assert(!heap_.empty());
//     del(0);
// }

// void HeapTimer::clear() {
//     ref_.clear();
//     heap_.clear();
// }

// int HeapTimer::get_next_tick() {
//     tick();
//     size_t res = -1;
//     if(!heap_.empty()) {
//         res = std::chrono::duration_cast<MS>(heap_.front().expires - Clock::now()).count();
//         if(res < 0) { res = 0; }
//     }
//     return res;
// }