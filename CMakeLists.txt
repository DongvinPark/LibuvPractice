cmake_minimum_required(VERSION 3.10)
project(LibuvPractice C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Use vcpkg toolchain (for include/link directories)
# IMPORTANT: when running cmake, add -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake

find_package(PkgConfig REQUIRED)

# libuv
pkg_check_modules(LIBUV REQUIRED libuv)
include_directories(${LIBUV_INCLUDE_DIRS})
link_directories(${LIBUV_LIBRARY_DIRS})

# glib
pkg_check_modules(GLIB REQUIRED glib-2.0)
include_directories(${GLIB_INCLUDE_DIRS})
link_directories(${GLIB_LIBRARY_DIRS})

# Add source files
add_executable(LibuvPractice src/main.c
        src/periodic_task.c
        src/periodic_task.h
        src/delayed_executor/delayed_executor.h
        src/delayed_executor/delayed_executor.c
        src/util/util.h
        src/util/util.c)

# Link against both libraries
target_link_libraries(LibuvPractice ${LIBUV_LIBRARIES} ${GLIB_LIBRARIES})
