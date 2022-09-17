add_subdirectory(${THIRD_PARTY_DIR}/glfw third_party/glfw)

set(DAWN_ENABLE_D3D12 OFF)
set(DAWN_ENABLE_METAL OFF)
set(DAWN_ENABLE_NULL OFF)
set(DAWN_ENABLE_DESKTOP_GL OFF)
set(DAWN_ENABLE_OPENGLES OFF)
set(DAWN_BUILD_SAMPLES OFF)
set(DAWN_BUILD_NODE_BINDINGS OFF)

add_subdirectory(${THIRD_PARTY_DIR}/dawn third_party/dawn)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS *.cpp)
file(GLOB_RECURSE headers CONFIGURE_DEPENDS *.h)

add_executable(native ${sources} ${headers})
add_compile_options(native PRIVATE
        -Werror
        -Wall
        -Wextra)
target_include_directories(native PRIVATE ../)
target_link_libraries(native PRIVATE
        glfw
        dawn_public_config
        dawn_native
        dawn_proc
        app)