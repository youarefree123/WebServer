{
    files = {
        "src/http/http_conn.cc"
    },
    depfiles_gcc = "build/.objs/test/linux/x86_64/debug/src/http/http_conn.cc.o:  src/http/http_conn.cc include/http/http_conn.h include/common/log.h  include/common/block_queue.h include/common/buffer.h  include/common/sql_conn_RAII.h include/common/sql_conn_pool.h  include/http/http_request.h include/http/http_response.h\
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