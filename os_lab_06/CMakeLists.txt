cmake_minimum_required(VERSION 3.14)
project(os_lab_06)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 17)
find_package(PkgConfig)
find_path(ZeroMQ_INCLUDE_DIR
        NAMES zmq.hpp
        PATHS ${PC_ZeroMQ_INCLUDE_DIRS}
		)
find_library(ZeroMQ_LIBRARY
        NAMES zmq
        PATHS ${PC_ZeroMQ_LIBRARY_DIRS}
		)

add_library(api os06_api.cpp os06_api.h)
target_link_libraries(api PUBLIC ${ZeroMQ_LIBRARY})
target_include_directories(api PUBLIC ${ZeroMQ_INCLUDE_DIR})

add_executable(child_main child_main.cpp)
target_include_directories(child_main PUBLIC ${ZeroMQ_INCLUDE_DIR})
target_link_libraries(child_main PUBLIC ${ZeroMQ_LIBRARY})
target_link_libraries(child_main PUBLIC api)

add_executable(main main.cpp)
target_include_directories(main PUBLIC ${ZeroMQ_INCLUDE_DIR})
target_link_libraries(main PUBLIC ${ZeroMQ_LIBRARY})
target_link_libraries(main PUBLIC api)




