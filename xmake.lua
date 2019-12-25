add_syslinks("pthread","gxiapi")
add_requires("opencv")

includes("Main")
includes("Interface")
includes("Share")
includes("Detector")
includes("Infantry")
includes("Sentry")
includes("Hero")
includes("Buff")


target("CV")
    set_kind("binary")
    set_rules("mode.debug", "mode.release")
    set_optimize("fastest")
    add_packages("opencv")

set_languages("c99", "cxx11")