find_package(Yarn REQUIRED)

set(WEBIDL_BINDER_BIN ${EMSCRIPTEN_ROOT_PATH}/tools/webidl_binder)
set(WEBPACK_BIN ${CMAKE_CURRENT_SOURCE_DIR}/node_modules/.bin/webpack)

set(BUNDLE_DIR ${CMAKE_BINARY_DIR}/bundle)
set(GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)

file(MAKE_DIRECTORY ${GEN_DIR})
file(MAKE_DIRECTORY ${BUNDLE_DIR})

add_custom_command(
        COMMENT "Generating Emscripten binding code from the WebIDL app file"
        OUTPUT ${GEN_DIR}/app.bind.cpp ${GEN_DIR}/app.bind.js
        DEPENDS app.idl
        COMMAND ${WEBIDL_BINDER_BIN} app.idl ${GEN_DIR}/app.bind
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        VERBATIM)

add_custom_target(generate_emscripten_bindings
        DEPENDS ${GEN_DIR}/app.bind.cpp ${GEN_DIR}/app.bind.js)

file(GLOB_RECURSE headers CONFIGURE_DEPENDS *.h)
file(GLOB_RECURSE sources CONFIGURE_DEPENDS *.cpp)

add_executable(wasm EXCLUDE_FROM_ALL ${sources} ${headers})
add_compile_options(wasm PRIVATE
        -Werror
        -Wall
        -Wextra)
target_link_options(wasm PRIVATE
        -sSTRICT
        -sENVIRONMENT=web
        -sWASM
        -sMODULARIZE
        -sASYNCIFY
        # Runtime options.
        -sUSE_WEBGPU
        -sTEXTDECODER=2
        -sNO_FILESYSTEM
        -sEXPORT_ES6
        -sEXPORT_NAME=createModule
        -sEXPORTED_RUNTIME_METHODS='[cwrap, ccall]'
        -sNO_INVOKE_RUN
        -sNO_MINIMAL_RUNTIME
        --no-entry
        --post-js ${GEN_DIR}/app.bind.js
        # Behaviour options.
        -sNO_ASSERTIONS
        -sABORTING_MALLOC
        -sALLOW_MEMORY_GROWTH
        -sNO_SUPPORT_ERRNO
        -sMALLOC=emmalloc
        # Output options.
        --output_eol=linux
        -O3
        -flto)

set_target_properties(wasm PROPERTIES OUTPUT_NAME app)
target_include_directories(wasm PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ..)
target_link_libraries(wasm PRIVATE app)
add_dependencies(wasm generate_emscripten_bindings)

add_custom_command(
        COMMENT "Installing Node modules using Yarn"
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/yarn.lock
        DEPENDS package.json
        COMMAND ${YARN_BIN} install
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        VERBATIM)

add_custom_target(install_node_modules
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/yarn.lock)

add_custom_command(
        COMMENT "Bundling Web application"
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/webpack.stamp
        DEPENDS webpack.js install_node_modules wasm
        COMMAND ${WEBPACK_BIN}
            --env INPUT_DIR=${CMAKE_CURRENT_SOURCE_DIR}
            --env MODULE_DIR=${CMAKE_CURRENT_BINARY_DIR}
            --env OUTPUT_DIR=${BUNDLE_DIR}
            --config=webpack.js
        COMMAND ${CMAKE_COMMAND}
            -E touch
            ${CMAKE_CURRENT_BINARY_DIR}/webpack.stamp
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        VERBATIM)

add_custom_target(bundle ALL
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/webpack.stamp)