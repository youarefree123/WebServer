{
    files = {
        "src/common/sql_conn_pool.cc"
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
    },
    depfiles_gcc = "build/.objs/test/linux/x86_64/debug/src/common/sql_conn_pool.cc.o:  src/common/sql_conn_pool.cc include/common/sql_conn_pool.h  include/common/log.h include/common/block_queue.h  include/common/buffer.h\
"
}