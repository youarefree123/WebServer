{
    values = {
        "/usr/bin/gcc",
        {
            "-m64",
            "-g",
            "-Wall",
            "-Werror",
            "-O0",
            "-std=c++14",
            "-Iinclude",
            "-Iinclude/common",
            "-Iinclude/http"
        }
    },
    files = {
        "src/common/thread_pool.cc"
    },
    depfiles_gcc = "build/.objs/test/linux/x86_64/debug/src/common/thread_pool.cc.o:  src/common/thread_pool.cc\
"
}