
#
# External dependencies
#

find_package(${META_PROJECT_NAME} REQUIRED HINTS "${CMAKE_CURRENT_SOURCE_DIR}/../../../")

#
# Executable name and options
#

# Target name
set(target lc3_test)
message(STATUS "Test ${target}")


#
# Sources
#

set(sources
    lc3_assemble_test.cpp
    lc3_basic_test.cpp
    lc3_parser_test.cpp
    lc3_params_test.cpp
    lc3_revision_2019_test.cpp
    lc3_plugins_test.cpp
    lc3_replay_test.cpp
)


#
# Create executable
#

# Build executable
add_executable(${target}
    ${sources}
)

# Create namespaced alias
add_executable(${META_PROJECT_NAME}::${target} ALIAS ${target})


#
# Project options
#

set_target_properties(${target}
    PROPERTIES
    ${DEFAULT_PROJECT_OPTIONS}
    FOLDER "${IDE_FOLDER}"
)


#
# Include directories
#

target_include_directories(${target}
    PRIVATE
    ${DEFAULT_INCLUDE_DIRECTORIES}
    ${Boost_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
    ${PROJECT_BINARY_DIR}/source/include
)


#
# Libraries
#

target_link_libraries(${target}
    PRIVATE
    ${DEFAULT_LIBRARIES}
    ${META_PROJECT_NAME}::lc3
    ${META_PROJECT_NAME}::lc3_replay
    ${Boost_LIBRARIES}
    ${ZLIB_LIBRARIES}
)


#
# Compile definitions
#

target_compile_definitions(${target}
    PRIVATE
    ${DEFAULT_COMPILE_DEFINITIONS}
    BOOST_TEST_DYN_LINK=1
)


#
# Compile options
#

target_compile_options(${target}
    PRIVATE
    ${DEFAULT_COMPILE_OPTIONS}
)


#
# Linker options
#

target_link_libraries(${target}
    PRIVATE
    ${DEFAULT_LINKER_OPTIONS}
)
