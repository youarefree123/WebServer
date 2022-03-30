-- includes sub-projects
target("test")
    set_kind("binary")
    add_files("./*.cc","common/*.cc","http/*.cc")