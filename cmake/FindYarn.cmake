set(_yarn_bin "${YARN_BIN}")

find_program(YARN_BIN yarn DOC "Path to Yarn")

if (YARN_BIN)
    if (NOT _yarn_bin)
        message(STATUS "Found Yarn binary: ${YARN_BIN}")
    endif()
    set(Yarn_FOUND TRUE CACHE INTERNAL "")
else ()
    set(Yarn_FOUND FALSE CACHE INTERNAL "")

    if(Yarn_FIND_REQUIRED)
        message(FATAL_ERROR "Failed to find a Yarn binary")
    endif()
endif()