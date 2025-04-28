cmake_minimum_required(VERSION 3.10)
project(LibuvPractice C)

# Use C language version to C11
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Link against libuv
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBUV REQUIRED libuv)

include_directories(${LIBUV_INCLUDE_DIRS})

# For macOS
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # Only needed on macOS
    link_directories(${LIBUV_LIBRARY_DIRS})
endif()

# Add source files
add_executable(LibuvPractice
        src/main.c
)

target_link_libraries(LibuvPractice ${LIBUV_LIBRARIES})