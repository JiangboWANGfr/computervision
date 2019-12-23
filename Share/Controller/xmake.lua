add_syslinks("pthread","gxiapi")
add_requires("opencv")

includes("Camera")
includes("Detector")
includes("PictureManipulator")
includes("SerialPort")
includes("Share")
includes("Socket")


target("ComputerVision")
    set_kind("binary")
    set_rules("mode.debug", "mode.release")
    set_optimize("fastest")
    add_includedirs("include")
    add_files("src/*.cpp")
    add_packages("opencv")