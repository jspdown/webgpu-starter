file(GLOB_RECURSE sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(app STATIC ${sources} ${headers})
add_compile_options(app PRIVATE
        -Werror
        -Wall
        -Wextra)
target_include_directories(app PUBLIC ${THIRD_PARTY_DIR})
