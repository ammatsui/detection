cmake_minimum_required(VERSION 3.1)

project(detection)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenCV REQUIRED)

add_executable(det "src/det.cpp")
target_include_directories(det PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(det ${OpenCV_LIBS})

add_executable(motdet "src/motdet.cpp")
target_include_directories(motdet PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(motdet ${OpenCV_LIBS})

