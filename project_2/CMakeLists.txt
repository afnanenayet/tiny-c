cmake_minimum_required(VERSION 3.1..3.12)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

# enable compile_commands for language servers
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(C_STANDARD C11)
set(program_name optimizer)

# enable all warnings
# set(CMAKE_C_FLAGS
#     "${CMAKE_C_FLAGS} -Wall -Wpedantic -std=c11"
#     )
 set (CMAKE_C_FLAGS_DEBUG
     "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG"
     )
# set (CMAKE_LINKER_FLAGS_DEBUG
#     "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address"
#     )

# optional doxygen package to generate docs
find_package(Doxygen)
doxygen_add_docs(
    docs
    ${PROJECT_SOURCE_DIR}
    COMMENT "Generate documentation"
    )

# add LLVM libraries
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
add_definitions(${LLVM_DEFINITIONS})
llvm_map_components_to_libnames(llvm_libs support core irreader)

include_directories(
    ${LLVM_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
add_executable(${program_name}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/optimizer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/llvm_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vec.c
    )
target_link_libraries(${program_name}
    ${llvm_libs}
    ${core}
    ${irreader}
    )

