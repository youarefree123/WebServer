{
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
    depfiles_gcc = "build/.objs/test/linux/x86_64/debug/src/epoller.cc.o: src/epoller.cc  include/epoller.h\
",
    files = {
        "src/epoller.cc"
    }
}