cmake_minimum_required(VERSION 3.0)
project(Cloth)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 17)


# TODO: add -flto once dawn fixes its ODR errors.
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g0 -DNDEBUG=1 -O3")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -D_DEBUG=1 -Wno-unused -O0")

file(GLOB sources src/*.cpp)
file(GLOB headers src/*.h)

# Disable all targets except Vulkan.
set(DAWN_ENABLE_D3D12 OFF)
set(DAWN_ENABLE_METAL OFF)
set(DAWN_ENABLE_NULL OFF)
set(DAWN_ENABLE_DESKTOP_GL OFF)
set(DAWN_ENABLE_OPENGLES OFF)

set(DAWN_BUILD_SAMPLES OFF)
set(DAWN_BUILD_NODE_BINDINGS OFF)

# Target specific builds.
if (EMSCRIPTEN)
	# Generate glue code from WebIDL definitions.
	add_custom_target(glue ALL COMMAND ${EMSCRIPTEN_ROOT_PATH}/tools/webidl_binder ${CMAKE_CURRENT_SOURCE_DIR}/src/target/web/app.idl glue)
	set(glue_cpp ${CMAKE_CURRENT_BINARY_DIR}/glue.cpp)
	set(glue_js ${CMAKE_CURRENT_BINARY_DIR}/glue.js)

	add_custom_target(html ALL)

	# Compute sources to WebAssembly.
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s EXPORT_NAME=createModule -s EXPORT_ES6=1 -s MODULARIZE=1 -s INVOKE_RUN=0")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s ENVIRONMENT=web -s WASM=1 -s USE_WEBGPU=1")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s EXPORTED_RUNTIME_METHODS='[cwrap, ccall]' --no-entry -s ASYNCIFY")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s ASSERTIONS=0 -s STRICT=1")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s MINIMAL_RUNTIME=0 -s TEXTDECODER=2 -s NO_FILESYSTEM=1 --output_eol=linux")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s ABORTING_MALLOC=0 -s ALLOW_MEMORY_GROWTH=0 -s SUPPORT_ERRNO=0 -s MALLOC=emmalloc")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --post-js ${glue_js}")

	file(GLOB_RECURSE platform_sources src/target/web/*)

	add_executable(app ${sources} ${platform_sources} ${glue_cpp_sources} ${headers})
	add_dependencies(app glue)
	target_include_directories(app PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

	# Bundle typescript sources.
	add_custom_target(bundle ALL COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/node_modules/.bin/webpack --env OUTPUT_DIR=${CMAKE_CURRENT_BINARY_DIR} --config=webpack.js WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
	add_dependencies(bundle app)
else ()
	add_subdirectory(lib/glfw EXCLUDE_FROM_ALL)
	add_subdirectory(lib/dawn EXCLUDE_FROM_ALL)

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
	set(CMAKE_CXX_CLANG_TIDY clang-tidy)

	file(GLOB_RECURSE platform_sources src/target/native/*.cpp)
	file(GLOB_RECURSE platform_headers src/target/native/*.h)

	message("sources" ${sources})
	message("platform_sources" ${platform_sources})
	message("platform_headers" ${platform_headers})
	message("headers" ${headers})

	add_executable(app ${sources} ${platform_sources} ${headers} ${platform_headers})

	target_link_libraries(app PRIVATE
			glfw
			dawn_public_config
			dawn_native
			dawn_proc)

endif()
