cmake_minimum_required(VERSION 3.10)
project(LibuvPractice C)

# Use C language version C11
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Platform-specific libuv handling
if (WIN32)
    # DO NOT use find_package(unofficial-libuv)
    # Just link directly, vcpkg handles include paths
    include_directories("C:/vcpkg/installed/x64-windows/include")
    link_directories("C:/vcpkg/installed/x64-windows/debug/lib")
    set(LIBUV_LIBRARIES uv)
else()
    # Use pkg-config on Unix-like systems
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBUV REQUIRED libuv)
    include_directories(${LIBUV_INCLUDE_DIRS})

    if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        link_directories(${LIBUV_LIBRARY_DIRS})
    endif()

    set(LIBUV_LIBRARIES ${LIBUV_LIBRARIES})
endif()

# Add source files
add_executable(LibuvPractice src/main.c)

# Link against libuv
target_link_libraries(LibuvPractice ${LIBUV_LIBRARIES})