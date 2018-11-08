cmake_minimum_required(VERSION 3.1..3.12)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

set(EXPORT_COMPILE_COMMANDS ON)
set(program_name RegAlloc)

# Enforce C++17 support
set(CMAKE_CXX_STANDARD 17)
set_property(TARGET ${program_name} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(CXX_EXTENSIONS OFF)

# add LLVM libraries
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
add_definitions(${LLVM_DEFINITIONS})

# for convenience when referencing the libraries
llvm_map_components_to_libnames(llvm_libs support core irreader)

include_directories(
    ${LLVM_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

add_executable(${program_name}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    )

target_link_libraries(${program_name}
    ${llvm_libs}
    ${core}
    ${irreader}
    )
