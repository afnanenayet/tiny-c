# CMakeLists.txt
# Afnan Enayet 2018

cmake_minimum_required(VERSION 3.1..3.12)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

# echo prefix paths for verification
# foreach(path ${CMAKE_PREFIX_PATH})
#   message("prefix path: " ${path})
# endforeach(path)

# enable compile_commands for language servers
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(C_STANDARD C11)
project(minic LANGUAGES C)
set(program_name minic)
include(CTest)

# enable all warnings
set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wall -Wpedantic -std=c11")

# look for bison and flex libraries
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# generate bison output
BISON_TARGET(MiniCParser
    ${PROJECT_SOURCE_DIR}/src/parser.y
    ${CMAKE_CURRENT_BINARY_DIR}/parser.c
    )

# generate flex output
FLEX_TARGET(MiniCScanner
    ${PROJECT_SOURCE_DIR}/src/lexer.l
    ${CMAKE_CURRENT_BINARY_DIR}/lexer.c
    )

ADD_FLEX_BISON_DEPENDENCY(MiniCScanner MiniCParser)

# only enable testing on debug builds
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    enable_testing()
    add_subdirectory(${PROJECT_SOURCE_DIR}/test)
ENDIF(CMAKE_BUILD_TYPE MATCHES Debug)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}
    )

# All source files are listed here. If a source file is added, add it here.
add_executable(${program_name}
    ${PROJECT_SOURCE_DIR}/src/minic.c
    ${PROJECT_SOURCE_DIR}/src/ast.c
    ${BISON_MiniCParser_OUTPUTS}
    ${FLEX_MiniCParser_OUTPUTS}
    )

TARGET_LINK_LIBRARIES(${program_name} ${FLEX_LIBRARIES})
