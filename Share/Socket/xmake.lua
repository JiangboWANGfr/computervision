add_syslinks("pthread","gxiapi")
add_requires("opencv")

includes("Share")

target("CV")
    set_kind("binary")
    set_rules("mode.debug", "mode.release")
    set_optimize("fastest")
    add_includedirs("include")
    add_files("src/*.cpp")
    add_packages("opencv")
