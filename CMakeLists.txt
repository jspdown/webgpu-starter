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
	set(WEB_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/target/web)
	# Generate glue code from WebIDL definition.
	add_custom_target(generate_glue ALL COMMAND ${EMSCRIPTEN_ROOT_PATH}/tools/webidl_binder ${WEB_SOURCE_DIR}/app.idl ${WEB_SOURCE_DIR}/glue.gen)

	# Generate typescript definition from WebIDL definition.
	add_custom_target(generate_ts_def ALL COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/node_modules/.bin/webidl2ts -i ${WEB_SOURCE_DIR}/app.idl -n app -ed -o ${WEB_SOURCE_DIR}/app.gen.d.ts)

	add_custom_target(generate)
	add_dependencies(generate generate_glue generate_ts_def)

	# Compute sources to WebAssembly.
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s EXPORT_NAME=createModule -s EXPORT_ES6=1 -s MODULARIZE=1 -s INVOKE_RUN=0")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s ENVIRONMENT=web -s WASM=1 -s USE_WEBGPU=1")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s EXPORTED_RUNTIME_METHODS='[cwrap, ccall]' --no-entry -s ASYNCIFY")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s ASSERTIONS=0 -s STRICT=1")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s MINIMAL_RUNTIME=0 -s TEXTDECODER=2 -s NO_FILESYSTEM=1 --output_eol=linux")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s ABORTING_MALLOC=0 -s ALLOW_MEMORY_GROWTH=0 -s SUPPORT_ERRNO=0 -s MALLOC=emmalloc")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --post-js ${WEB_SOURCE_DIR}/glue.gen.js")

	file(GLOB_RECURSE platform_sources ${WEB_SOURCE_DIR}/*.cpp)
	file(GLOB_RECURSE platform_headers ${WEB_SOURCE_DIR}/*.h)
	list(REMOVE_ITEM platform_sources ${WEB_SOURCE_DIR}/glue.gen.cpp)

	add_executable(app ${sources} ${platform_sources} ${headers} ${platform_headers})
	add_dependencies(app generate)
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

	add_executable(app ${sources} ${platform_sources} ${headers} ${platform_headers})

	target_link_libraries(app PRIVATE
			glfw
			dawn_public_config
			dawn_native
			dawn_proc)

endif()
