{
    depfiles_gcc = "build/.objs/test/linux/x86_64/debug/src/http/http_request.cc.o:  src/http/http_request.cc include/http/http_request.h  include/common/buffer.h include/common/log.h  include/common/block_queue.h include/common/sql_conn_pool.h  include/common/sql_conn_RAII.h\
",
    files = {
        "src/http/http_request.cc"
    },
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