cmake_minimum_required(VERSION 3.10)
project(main)

set(CMAKE_CXX_STANDARD 23)

add_executable(main main.cpp)

# 请将下面的路径替换为实际的include和lib路径
set(INCLUDE_PATH "C:/Program Files/vcpkg/installed/x64-windows/include")
set(LIB_PATH "C:/Program Files/vcpkg/installed/x64-windows/lib")

target_include_directories(main PRIVATE "${INCLUDE_PATH}")

target_link_libraries(main PRIVATE "${LIB_PATH}/glbinding.lib")
target_link_libraries(main PRIVATE "${LIB_PATH}/glm.lib")
target_link_libraries(main PRIVATE "${LIB_PATH}/glfw3dll.lib")
