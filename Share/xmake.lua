add_syslinks("pthread","gxiapi")
add_requires("opencv")

includes("Camera")
includes("Controller")
includes("SerialPort")  
includes("Socket")
includes("Interface")

target("CV")
    set_kind("binary")
    set_rules("mode.debug", "mode.release")
    set_optimize("fastest")
    add_packages("opencv")
