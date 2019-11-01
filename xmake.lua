add_syslinks("pthread","gxiapi")
add_requires("opencv")

includes("Camera")
includes("Controller")
includes("Detector")
includes("PictureManipulator")
includes("SerialPort")
includes("Share")
includes("Main")


target("ComputerVision")
    set_kind("binary")
    set_rules("mode.debug", "mode.release")
    set_optimize("fastest")
    -- on_load(function (target)
    --     import("lib.detect.find_package")
    --     target:add(find_package("opencv"))
    --     end)
    add_packages("opencv")

set_languages("c99", "cxx11")