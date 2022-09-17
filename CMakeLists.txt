cmake_minimum_required(VERSION 3.24)
project(webgpu-starter VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)

add_subdirectory(app)

if (EMSCRIPTEN)
    add_subdirectory(web)
else()
    add_subdirectory(native)
endif()