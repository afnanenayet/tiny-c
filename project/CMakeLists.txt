cmake_minimum_required(VERSION 3.1..3.12)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

# enable compile_commands.json for language servers
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(C_STANDARD C11)
project(minic LANGUAGES C)
set(program_name minic)
include(CTest)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# only enable testing on debug builds
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    enable_testing()
    add_subdirectory(${PROJECT_SOURCE_DIR}/test)
ENDIF(CMAKE_BUILD_TYPE MATCHES Debug)

file(GLOB sources "${PROJECT_SOURCE_DIR}/src/*.c")

# All source files are listed here. If a source file is added, add it here.
add_executable(${program_name} ${sources}) 
