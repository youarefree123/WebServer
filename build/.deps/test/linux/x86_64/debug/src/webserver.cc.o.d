{
    files = {
        "src/webserver.cc"
    },
    depfiles_gcc = "build/.objs/test/linux/x86_64/debug/src/webserver.cc.o: src/webserver.cc  include/webserver.h include/epoller.h include/common/log.h  include/common/block_queue.h include/common/buffer.h  include/common/heap_timer.h include/common/sql_conn_pool.h  include/common/thread_pool.h include/common/sql_conn_RAII.h  include/http/http_conn.h include/http/http_request.h  include/http/http_response.h\
",
    values = {
        "/usr/bin/gcc",
        {
            "-m64",
            "-g",
            "-Wall",
            "-O0",
            "-std=c++14",
            "-Iinclude",
            "-Iinclude/common",
            "-Iinclude/http"
        }
    }
}