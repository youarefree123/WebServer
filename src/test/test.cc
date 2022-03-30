// #include <iostream>
// #include <unistd.h>
// #include "thread_pool.h"
// #include "http_conn.h"
// #include "buffer.h"
// #include "block_queue.h"
// #include "log.h" 
// #include <features.h>
// #include <sys/stat.h>
// #if __GLIBC__ == 2 && __GLIBC_MINOR__ < 30
// #include <sys/syscall.h>
// #define gettid() syscall(SYS_gettid)
// #endif
// using std::cin;
// using std::cout;
// using std::endl;
// int tmp = 0;

// std::mutex mtx;

// void process_thread_pool() {
//     cout<<std::this_thread::get_id()<<endl;
// }

// void test_thread_pool() {
//     ThreadPool tp(4);
//     usleep(40);
//     for(int i = 0; i < 500; ++i) {
//         tp.add_task(process_thread_pool);
//     }
//     sleep(2);
// }

// void process_block_queue1(int& data, BlockDeque<int>& bd ) {
//     while(data < 1000) {
//         {
//             // std::lock_guard<std::mutex> locker(mtx);
//             bd.push_back(data);
//             cout<<std::this_thread::get_id()<<" push_back "<<data++<<endl;
//         }
//     }
// }

// void process_block_queue3(int& data, BlockDeque<int>& bd ) {
//     while(data < 1000) {
//         {
//             // std::lock_guard<std::mutex> locker(mtx);
//             bd.push_front(data);
//             cout<<std::this_thread::get_id()<<" push_front "<<data++<<endl;
//         } 
//     }
// }


// void process_block_queue2(BlockDeque<int>& bd ) {
//     while(bd.back() < 100){
//         {
//             // std::lock_guard<std::mutex> locker(mtx);
//             //  usleep(10);
//             int tmp = -1;
//             bd.pop(tmp,5);
//             cout<<std::this_thread::get_id()<<" pop_timeout "<<tmp<<endl;
//         }
        
//     }
    
// }

// void process_block_queue4(BlockDeque<int>& bd ) {
//     while(bd.back() < 100){
//         {
//             // std::lock_guard<std::mutex> locker(mtx);
//             int tmp = -1;
//             // usleep(10);
//             bd.pop(tmp);
//             cout<<std::this_thread::get_id()<<" pop "<<tmp<<endl;
//         }
        
//     }
    
// }

// void test_block_queue(){
//     std::vector< std::thread > threads(4);
//     usleep(20);
//     int i = 0;
//     BlockDeque<int> bd(500); // 最大容量20的阻塞队列
//     // bd.push_back(3);
//     // int tmp = -1;
//     // bd.pop(tmp,0);
//     // cout<<tmp<<endl;

//     threads[3] = std::thread(   process_block_queue4, std::ref(bd) );
//     threads[2] = std::thread(  process_block_queue2, std::ref(bd)  );
    
    
//     threads[1] = std::thread( process_block_queue3, std::ref(i), std::ref(bd) );
//     threads[0] = std::thread( process_block_queue1, std::ref(i), std::ref(bd) );
    
    
    
    
    
    
    
    

//     for(auto& t : threads) {
//         t.join();
//     }
//     // while( i < 100 ){ 
//     //     // usleep(10);
//     //     // cout<<"bd.front:"<<bd.front()<<" bd.back():"<<bd.back()<<endl;
//     // }
    
//     return;   
// }


// // int main(int argc, char** argv)
// // {
// //     // test_thread_pool();
// //     cout<<"test"<<endl;
// //     test_block_queue();
// //     sleep(10);
// //     return 0;
// // }




// void TestLog() {
//     int cnt = 0, level = 0;
//     Log::instance()->init(level, "./testlog1", ".log", 0);
//     for(level = 3; level >= 0; level--) {
//         Log::instance()->set_level(level);
//         for(int j = 0; j < 10000; j++ ){
//             for(int i = 0; i < 4; i++) {
//                 LOG_BASE(i,"%s 111111111 %d ============= ", "Test", cnt++);
//             }
//         }
//     }
//     cnt = 0;
//     Log::instance()->init(level, "./testlog2", ".log", 5000);
//     for(level = 0; level < 4; level++) {
//         Log::instance()->set_level(level);
//         for(int j = 0; j < 10000; j++ ){
//             for(int i = 0; i < 4; i++) {
//                 LOG_BASE(i,"%s 222222222 %d ============= ", "Test", cnt++);
//             }
//         }
//     }
// }

// void ThreadLogTask(int i, int cnt) {
//     for(int j = 0; j < 10000; j++ ){
//         LOG_BASE(i,"PID:[%04d]======= %05d ========= ", gettid(), cnt++);
//     }
// }

// void TestThreadPool() {
//     Log::instance()->init(0, "./testThreadpool", ".log", 5000);
//     ThreadPool threadpool(6);
//     for(int i = 0; i < 18; i++) {
//         threadpool.add_task(std::bind(ThreadLogTask, i % 4, i * 10000));
//     }
//     getchar();
// }

// int main() {

    
    
//     TestLog();
//     TestThreadPool();
//     // mkdir("log123",0777);
//     // FILE* fp_ = fopen("./log123/test.log","a");
//     // fprintf(fp_, "%s %s %s %d", "We", "are", "in", 2014);
// }