cmake_minimum_required(VERSION 3.25)
project(shellscribe VERSION 1.0.0 LANGUAGES C)

# Add the custom CMake modules directory
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

# Set include directories
include_directories(
${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Auto-discover all source files
file(GLOB_RECURSE SHELLSCRIBE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

file(GLOB_RECURSE SHELLSCRIBE_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)

# Create the main executable
add_executable(scribe ${SHELLSCRIBE_SOURCES})

# Set the compilation flags
target_compile_definitions(scribe PRIVATE
SHELLSCRIBE_VERSION="${PROJECT_VERSION}"
)

# Tests only if BUILD_TESTS is enabled
if(BUILD_TESTS)
include(CTest)
# add_subdirectory(tests)
endif()

# Install the executable
install(
TARGETS scribe
DESTINATION bin
)

# Configuring the CPack package
include(CPack)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Documentation made simple for your shell scripts!")
set(CPACK_PACKAGE_VENDOR "tomcdj71")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
