
#
# Platform and architecture setup
#

# Get upper case system name
string(TOUPPER ${CMAKE_SYSTEM_NAME} SYSTEM_NAME_UPPER)

option(OPTION_ENABLE_PROFILING "Enable Code Profiling compile options" OFF)

# Determine architecture (32/64 bit)
set(X64 OFF)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(X64 ON)
endif()


#
# Project options
#

set(DEFAULT_PROJECT_OPTIONS
    DEBUG_POSTFIX             "d"
    CXX_STANDARD              17
    LINKER_LANGUAGE           "CXX"
    POSITION_INDEPENDENT_CODE ON
    CXX_VISIBILITY_PRESET     "hidden"
    CXX_EXTENSIONS            OFF
)

# TODO Fix the visibility error on MacOSx
# Something strange with the plugins on MacOSx with the visibility set to Hidden.
if(APPLE)
    set(DEFAULT_PROJECT_OPTIONS
        DEBUG_POSTFIX             "d"
        CXX_STANDARD              17
        LINKER_LANGUAGE           "CXX"
        POSITION_INDEPENDENT_CODE ON
        CXX_VISIBILITY_PRESET     "default"
        CXX_EXTENSIONS            OFF
    )
endif()

#
# Include directories
#

set(DEFAULT_INCLUDE_DIRECTORIES)


#
# Libraries
#

set(DEFAULT_LIBRARIES)


#
# Compile definitions
#

set(DEFAULT_COMPILE_DEFINITIONS
    SYSTEM_${SYSTEM_NAME_UPPER}
)

# MSVC compiler options
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
    set(DEFAULT_COMPILE_DEFINITIONS ${DEFAULT_COMPILE_DEFINITIONS}
        _SCL_SECURE_NO_WARNINGS  # Calling any one of the potentially unsafe methods in the Standard C++ Library
        _CRT_SECURE_NO_WARNINGS  # Calling any one of the potentially unsafe methods in the CRT Library
    )
endif ()


#
# Compile options
#

set(DEFAULT_COMPILE_OPTIONS)

# MSVC compiler options
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
    set(DEFAULT_COMPILE_OPTIONS ${DEFAULT_COMPILE_OPTIONS}
        PRIVATE
            /MP           # -> build with multiple processes
            /W4           # -> warning level 4
            # /WX         # -> treat warnings as errors

            #$<$<CONFIG:Debug>:
            #/RTCc         # -> value is assigned to a smaller data type and results in a data loss
            #>

            $<$<CONFIG:Release>:
            /Gw           # -> whole program global optimization
            /GS-          # -> buffer security check: no
            /GL           # -> whole program optimization: enable link-time code generation (disables Zi)
            /GF           # -> enable string pooling
            >

            /std:c++17
        PUBLIC
            /wd4251       # -> disable warning: 'identifier': class 'type' needs to have dll-interface to be used by clients of class 'type2'
            /wd4592       # -> disable warning: 'identifier': symbol will be dynamically initialized (implementation limitation)
            /wd4244       # -> disable warning: possible loss of data
            /wd4245       # -> disable warning: possible loss of data
            /wd4267       # -> disable warning: possible loss of data
            # /wd4201     # -> disable warning: nonstandard extension used: nameless struct/union (caused by GLM)
            # /wd4127     # -> disable warning: conditional expression is constant (caused by Qt)
    )
endif ()

# GCC and Clang compiler options
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(DEFAULT_COMPILE_OPTIONS ${DEFAULT_COMPILE_OPTIONS}
        PRIVATE
            -Wall
            -Wextra
            -Wunused

            -Wreorder
            -Wignored-qualifiers
            -Wmissing-braces
            -Wreturn-type
            -Wswitch
            -Wswitch-default
            -Wuninitialized
            -Wmissing-field-initializers

            $<$<CXX_COMPILER_ID:GNU>:
                -Wmaybe-uninitialized

                $<$<VERSION_GREATER:$<CXX_COMPILER_VERSION>,4.8>:
                    -Wpedantic

                    -Wreturn-local-addr
                >
            >

            $<$<CXX_COMPILER_ID:Clang>:
                -Wpedantic

                # -Wreturn-stack-address # gives false positives
            >

            $<$<BOOL:${OPTION_COVERAGE_ENABLED}>:
                -fprofile-arcs
                -ftest-coverage
            >

            $<$<BOOL:${OPTION_ENABLE_PROFILING}>:
                -pg
            >
            -std=c++17
        PUBLIC
            $<$<PLATFORM_ID:Darwin>:
                -pthread
            >
    )
endif ()


#
# Linker options
#

set(DEFAULT_LINKER_OPTIONS)

# Use pthreads on mingw and linux
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU" OR "${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    set(DEFAULT_LINKER_OPTIONS
        PUBLIC
            ${DEFAULT_LINKER_OPTIONS}
            -pthread
    )

    if (${OPTION_COVERAGE_ENABLED})
        set(DEFAULT_LINKER_OPTIONS
            PUBLIC
                ${DEFAULT_LINKER_OPTIONS}
                -fprofile-arcs
                -ftest-coverage
        )
    endif()

    if (${OPTION_ENABLE_PROFILING})
        set(DEFAULT_LINKER_OPTIONS
            PUBLIC
                ${DEFAULT_LINKER_OPTIONS}
                -pg
        )
    endif()
endif()

# Use -flat_namespace on OSX this is apparently needed due to plugins not working well i.e. InstructionPlugins won't be recognized as such.
if (APPLE)
        set(DEFAULT_LINKER_OPTIONS
            PUBLIC
                ${DEFAULT_LINKER_OPTIONS}
                -flat_namespace
        )
endif()