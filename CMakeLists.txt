cmake_minimum_required(VERSION 3.20.0)
project(Lumos)

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