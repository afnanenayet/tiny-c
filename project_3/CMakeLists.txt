cmake_minimum_required(VERSION 3.1..3.12)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

set(program_name RegAlloc)
set(EXPORT_COMPILE_COMMANDS ON)

# Enforce C++17 support
set(CMAKE_CXX_STANDARD 17)
set_property(TARGET PROPERTY CXX_STANDARD_REQUIRED ON)

# add LLVM libraries
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
add_definitions(${LLVM_DEFINITIONS})

# Clang doesn't require any flags to link std::filesystem, but GCC does
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_FXX_FLAGS} -Werror -Wall -Wpedantic")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_FXX_FLAGS} -Werror -Wall -Wpedantic -lstdc++fs")
endif()

# for convenience when referencing the libraries
llvm_map_components_to_libnames(llvm_libs support core irreader)

include_directories(
    ${LLVM_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

add_executable(${program_name}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/LLVMUtils.cpp
    )

target_link_libraries(${program_name}
    ${llvm_libs}
    ${core}
    ${irreader}
    )
