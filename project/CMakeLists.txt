# CMakeLists.txt
# Afnan Enayet 2018

cmake_minimum_required(VERSION 3.1..3.12)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

# enable compile_commands for language servers
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(C_STANDARD C11)
project(minic LANGUAGES C)
set(program_name minic)
include(CTest)

# enable all warnings
set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wall -Wpedantic -std=c11")

# look for bison and flex libraries
find_package(BISON 3.1 REQUIRED)
find_package(FLEX 2.6 REQUIRED)

# optional doxygen package to generate docs
find_package(Doxygen)

doxygen_add_docs(
    docs
    ${PROJECT_SOURCE_DIR}
    COMMENT "Generate documentation"
    )

# generate bison output
BISON_TARGET(MiniCParser
    ${PROJECT_SOURCE_DIR}/src/parser.y
    ${CMAKE_CURRENT_BINARY_DIR}/parser.c
    )

# generate flex output
FLEX_TARGET(MiniCScanner
    ${PROJECT_SOURCE_DIR}/src/lexer.l
    ${CMAKE_CURRENT_BINARY_DIR}/scanner.c
    )

ADD_FLEX_BISON_DEPENDENCY(MiniCScanner MiniCParser)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}
    )

# All source files are listed here. If a source file is added, add it here.
add_executable(${program_name}
    ${PROJECT_SOURCE_DIR}/src/ast.c
    ${PROJECT_SOURCE_DIR}/src/minic.c
    ${PROJECT_SOURCE_DIR}/src/vector.c
    ${BISON_MiniCParser_OUTPUTS}
    ${FLEX_MiniCScanner_OUTPUTS}
    )

# creating a library for the unit tests
add_library(minic_lib
    ${PROJECT_SOURCE_DIR}/src/ast.c
    ${PROJECT_SOURCE_DIR}/src/minic.c
    ${PROJECT_SOURCE_DIR}/src/vector.c
    )
