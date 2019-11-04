add_syslinks("pthread","gxiapi")
add_requires("opencv")

target("ComputerVision")
    set_kind("binary")
    set_rules("mode.debug", "mode.release")
    set_optimize("fastest")
    add_includedirs(".")
    add_packages("opencv")

set_languages("c99", "cxx11")