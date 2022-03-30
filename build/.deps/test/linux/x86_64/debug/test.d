{
    files = {
        "build/.objs/test/linux/x86_64/debug/src/main.cc.o",
        "build/.objs/test/linux/x86_64/debug/src/webserver.cc.o",
        "build/.objs/test/linux/x86_64/debug/src/epoller.cc.o",
        "build/.objs/test/linux/x86_64/debug/src/common/sql_conn_pool.cc.o",
        "build/.objs/test/linux/x86_64/debug/src/common/log.cc.o",
        "build/.objs/test/linux/x86_64/debug/src/common/heap_timer.cc.o",
        "build/.objs/test/linux/x86_64/debug/src/common/buffer.cc.o",
        "build/.objs/test/linux/x86_64/debug/src/http/http_conn.cc.o",
        "build/.objs/test/linux/x86_64/debug/src/http/http_response.cc.o",
        "build/.objs/test/linux/x86_64/debug/src/http/http_request.cc.o"
    },
    values = {
        "/usr/bin/g++",
        {
            "-m64",
            "-lpthread",
            "-lmysqlclient"
        }
    }
}