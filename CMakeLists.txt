cmake_minimum_required(VERSION 3.20)
project(WebScreenShare LANGUAGES C CXX)

# C++ settings
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Collect source files
file(GLOB_RECURSE CPP_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.cpp
)

set(SOURCES ${CPP_SOURCES} ${C_SOURCES})

set(CMAKE_CXX_FLAGS "-D__STDC_CONSTANT_MACROS")

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})
target_compile_definitions(${PROJECT_NAME} PRIVATE
    PROJECT_DIR="${CMAKE_SOURCE_DIR}"
)

# Include base directories
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Packages.cmake")

# Link Time Optimization
include(CheckIPOSupported)
check_ipo_supported(RESULT result)
if(result)
    set_target_properties(${PROJECT_NAME}
    PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

# ASAN
set(
    CMAKE_CXX_FLAGS_DEBUG
    "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address"
)
