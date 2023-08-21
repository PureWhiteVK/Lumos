# for $<TARGET_RUNTIME_DLLS:tgt> generator-expression
cmake_minimum_required(VERSION 3.21.0)
project(Lumos)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin)
set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options("/utf-8")
endif()

# used with command `cmake -DENABLE_TESTING=ON -B build -S .`
# Define an option with default value OFF (disabled)
option(ENABLE_TESTING "Enable testing" OFF)

cmake_path(SET DATA_PATH NORMALIZE ${PROJECT_SOURCE_DIR}/data)

add_subdirectory(cmake)
add_subdirectory(extern)
add_subdirectory(src)

if (ENABLE_TESTING)
  enable_testing()
  add_subdirectory(test)
endif()