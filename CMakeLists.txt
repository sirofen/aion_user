cmake_minimum_required(VERSION 3.16)
project(user_mode)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "/std:c++latest")
set(CMAKE_CXX_STANDARD 23)

message(${CMAKE_BINARY_DIR})

set(SOURCES
        src/aik/internal/driver.cpp
        src/aik/internal/process_module.cpp
        src/aik/internal/process.cpp
        src/aik/internal/page.cpp
        src/aik/aik_process.cpp
        src/aik/aion_write_builder.cpp
        src/aik/aion_write.cpp
        src/aik/client_values.cpp
        src/aik/utils/utils.cpp)

add_executable(user_mode main.cpp ${SOURCES})
target_include_directories(user_mode PRIVATE include)

# POST_BUILD patcher
include(cmake/PatchBinary.cmake)
