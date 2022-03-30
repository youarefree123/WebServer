{
    files = {
        "src/http/http_response.cc"
    },
    depfiles_gcc = "build/.objs/test/linux/x86_64/debug/src/http/http_response.cc.o:  src/http/http_response.cc include/http/http_response.h  include/common/buffer.h include/common/log.h  include/common/block_queue.h\
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