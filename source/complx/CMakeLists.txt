#
# External dependencies
#

if(UNIX)
    include(FindPkgConfig)
    pkg_check_modules(GLIB2 REQUIRED glib-2.0)
endif(UNIX)

find_package(wxWidgets COMPONENTS aui propgrid stc adv core base)

#
# Testing
#

set(target complx_lib)

# Exit here if required dependencies are not met
if (NOT wxWidgets_FOUND)
    message(STATUS "Program complx skipped: wxWidgets not found")
    return()
else()
    message(STATUS "Program complx")
endif()

include(${wxWidgets_USE_FILE})

#
# Sources
#

set(sources
    ComplxFrame.cpp
    MemoryView.cpp
    data/Lc3BinaryDisplayData.cpp
    data/MemoryViewBinaryDataRenderer.cpp
    data/MemoryViewInfoDataRenderer.cpp
    data/MemoryViewDataModel.cpp
    data/ProcessStatusRegisterProperty.cpp
    data/RegisterProperty.cpp
    gen/ComplxFrameDecl.cpp
    util/GuiConstants.cpp
    util/ValidationHelper.cpp
)

#
# Headers
#

set(headers
    ComplxFrame.hpp
    MemoryView.hpp
    LoadingOptions.hpp
    ExecuteOptions.hpp
    data/Lc3BinaryDisplayData.hpp
    data/MemoryViewBinaryDataRenderer.hpp
    data/MemoryViewInfoDataRenderer.hpp
    data/MemoryViewDataModel.hpp
    data/ProcessStatusRegisterProperty.hpp
    data/RegisterProperty.hpp
    data/PropertyTypes.hpp
    gen/ComplxFrameDecl.h
    util/GuiConstants.hpp
    util/ValidationHelper.hpp
)

# Build library
add_library(${target}
    ${sources}
    ${headers}
)

# Create namespaced alias
add_library(${META_PROJECT_NAME}::${target} ALIAS ${target})

# Export library for downstream projects
export(TARGETS ${target} NAMESPACE ${META_PROJECT_NAME}:: FILE ${PROJECT_BINARY_DIR}/cmake/${target}/${target}-export.cmake)

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
    ${PROJECT_BINARY_DIR}/source/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}

    PUBLIC
    ${DEFAULT_INCLUDE_DIRECTORIES}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${wxWidgets_INCLUDE_DIRS}
    ${GLIB2_INCLUDE_DIRS}

    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:include>
)

#
# Libraries
#

target_link_libraries(${target}
    PUBLIC
    ${DEFAULT_LIBRARIES}
)


#
# Compile definitions
#

target_compile_definitions(${target}
    PUBLIC
    ${DEFAULT_COMPILE_DEFINITIONS}
)


#
# Compile options
#

target_compile_options(${target}
    PUBLIC
    ${DEFAULT_COMPILE_OPTIONS}
)


#
# Linker options
#

target_link_libraries(${target}
    PUBLIC
    ${DEFAULT_LINKER_OPTIONS}
    ${wxWidgets_LIBRARIES}
    ${GLIB2_LIBRARIES}
    ${META_PROJECT_NAME}::lc3
    ${META_PROJECT_NAME}::logging
)

perform_health_checks(
    ${target}
    ${sources}
    ${headers}
)

#
# Executable name and options
#

# Target name
set(target complx)

#
# Create executable
#

# Build executable
add_executable(${target}
    MACOSX_BUNDLE
    ComplxApp.cpp
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
    ${PROJECT_BINARY_DIR}/source/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/include

    PUBLIC
    ${DEFAULT_INCLUDE_DIRECTORIES}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${wxWidgets_INCLUDE_DIRS}
    ${GLIB2_INCLUDE_DIRS}

    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:include>
)


#
# Libraries
#

target_link_libraries(${target}
    PRIVATE
    ${DEFAULT_LIBRARIES}
    ${wxWidgets_LIBRARIES}
    ${GLIB2_LIBRARIES}
    ${META_PROJECT_NAME}::lc3
    ${META_PROJECT_NAME}::logging
    ${META_PROJECT_NAME}::complx_lib
)


#
# Compile definitions
#

target_compile_definitions(${target}
    PRIVATE
    ${DEFAULT_COMPILE_DEFINITIONS}
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


#
# Target Health
#

perform_health_checks(
    ${target}
    ${sources}
)


#
# Deployment
#

# Executable
install(TARGETS ${target}
    RUNTIME DESTINATION ${INSTALL_BIN} COMPONENT runtime
    BUNDLE  DESTINATION ${INSTALL_BIN} COMPONENT runtime
)