{
    files = {
        "src/common/heap_timer.cc"
    },
    depfiles_gcc = "build/.objs/test/linux/x86_64/debug/src/common/heap_timer.cc.o:  src/common/heap_timer.cc include/common/heap_timer.h  include/common/log.h include/common/block_queue.h  include/common/buffer.h\
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