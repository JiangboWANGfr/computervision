#输入最低版本要求，一般是这个，反正我没改
cmake_minimum_required(VERSION 2.8)

#C11标准，现在一般都用这个
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

#输入项目名字，也就是文件夹的名字
project(BuffDetection)

#Release模式编译更快，也可以使用Debug模式试一下
if(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release)
endif()

add_subdirectory(src build)

#vscode的opencv默认路径为"/usr/local/share/OpenCV"
