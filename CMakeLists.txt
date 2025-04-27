cmake_minimum_required(VERSION 3.10)
project(LibuvPractice C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Add source files
add_executable(LibuvPractice
        src/main.c
)

# Link against libuv
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBUV REQUIRED libuv)

include_directories(${LIBUV_INCLUDE_DIRS})
target_link_libraries(LibuvPractice ${LIBUV_LIBRARIES})

