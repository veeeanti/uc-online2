cmake_minimum_required(VERSION 3.16)
project(uc-online2 VERSION 1.0.4 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    dllmain.cpp
)

set(HEADERS
    include/platform.h
    include/globals.h
    include/callback_dispatcher.h
    include/dll_loader.h
    include/dump_handler.h
    include/registfuncs.h
    include/resource.h
    include/api/api_callbacks.h
    include/api/api_client.h
    include/api/api_interfaces.h
    include/api/api_interfaces_v.h
    include/api/api_gameserver.h
    include/api/api_breakpad.h
    include/api/api_shutdown.h
    include/api/api_factory.h
    include/api/api_flat.h
    include/sdk/steam_api.h
    include/sdk/steam_api_common.h
    include/sdk/steam_api_internal.h
    include/sdk/steamtypes.h
    include/sdk/steamclientpublic.h
    include/sdk/steam_gameserver.h
)

# Determine output name based on platform and architecture
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH_SUFFIX "64")
else()
    set(ARCH_SUFFIX "")
endif()

if(WIN32)
    set(TARGET_NAME "steam_api${ARCH_SUFFIX}")
else()
    set(TARGET_NAME "steam_api${ARCH_SUFFIX}")
endif()

add_library(${TARGET_NAME} SHARED ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/sdk
    ${CMAKE_CURRENT_SOURCE_DIR}/include/api
)

# Preprocessor definitions
target_compile_definitions(${TARGET_NAME} PRIVATE
    STEAM_API_EXPORTS
    _CRT_SECURE_NO_WARNINGS
    $<$<CONFIG:Release>:NDEBUG>
    $<$<CONFIG:Debug>:_DEBUG>
)

# Platform-specific settings
if(WIN32)
    target_compile_definitions(${TARGET_NAME} PRIVATE
        WIN32
        _WINDOWS
        _USRDLL
    )

    target_link_libraries(${TARGET_NAME} PRIVATE Shlwapi.lib)

    # Resource file (Windows only)
    target_sources(${TARGET_NAME} PRIVATE uc_online2.rc)

    # MSVC-specific flags
    if(MSVC)
        target_compile_options(${TARGET_NAME} PRIVATE
            /O2          # MaxSpeed optimization
            /Gw          # Function-level linking
            /GL          # Whole program optimization
            /guard:cf-   # Disable Control Flow Guard
            /MT          # Static CRT (MultiThreaded)
            /W3          # Warning level 3
        )
        target_link_options(${TARGET_NAME} PRIVATE
            /LTCG        # Link-time code generation
            /OPT:REF     # Optimize references
            /OPT:ICF     # COMDAT folding
        )
    endif()

    # Win32 specific defines
    if(CMAKE_SIZEOF_VOID_P EQUAL 4)
        target_compile_definitions(${TARGET_NAME} PRIVATE WIN32)
    endif()

elseif(UNIX)
    # GCC/Clang flags
    target_compile_options(${TARGET_NAME} PRIVATE
        -O2
        -fPIC
        -fvisibility=hidden
        -Wall
        -Wno-unused-variable
        -Wno-unused-function
        -Wno-write-strings
    )

    # Link against dl (dlopen/dlsym) and pthread
    target_link_libraries(${TARGET_NAME} PRIVATE dl pthread)

    # Set RPATH so the library can find dependencies
    set_target_properties(${TARGET_NAME} PROPERTIES
        INSTALL_RPATH "$ORIGIN"
        BUILD_WITH_INSTALL_RPATH FALSE
    )
endif()

# Output directory
set_target_properties(${TARGET_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<IF:$<BOOL:${ARCH_SUFFIX}>,x64,x86>
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<IF:$<BOOL:${ARCH_SUFFIX}>,x64,x86>
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<IF:$<BOOL:${ARCH_SUFFIX}>,x64,x86>
)

# Strip release builds on Linux/macOS
if(UNIX AND CMAKE_BUILD_TYPE STREQUAL "Release")
    if(APPLE)
        # -x strips only local symbols; avoids indirect symbol table errors on macOS dylibs
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_STRIP} -x $<TARGET_FILE:${TARGET_NAME}>
            COMMENT "Stripping ${TARGET_NAME}"
        )
    else()
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_STRIP} $<TARGET_FILE:${TARGET_NAME}>
            COMMENT "Stripping ${TARGET_NAME}"
        )
    endif()
endif()
