{
    files = {
        "src/common/log.cc"
    },
    depfiles_gcc = "build/.objs/test/linux/x86_64/debug/src/common/log.cc.o:  src/common/log.cc include/common/log.h include/common/block_queue.h  include/common/buffer.h\
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