{
    files = {
        "src/common/buffer.cc"
    },
    depfiles_gcc = "build/.objs/test/linux/x86_64/debug/src/common/buffer.cc.o:  src/common/buffer.cc include/common/buffer.h\
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