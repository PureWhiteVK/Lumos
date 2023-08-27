include(FetchContent)

function(add_glfw SRC_DIR BIN_DIR)
  set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "")
  set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "")
  set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "")
  set(GLFW_INSTALL OFF CACHE INTERNAL "")
  add_subdirectory(${SRC_DIR} ${BIN_DIR})
  add_library(glfw::glfw ALIAS glfw)
endfunction(add_glfw)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.3.8
  GIT_SHALLOW    ON
)
FetchContent_Populate(glfw)
add_glfw(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR})

FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        v1.89.8-docking
  GIT_SHALLOW    ON
)
FetchContent_Populate(imgui)
add_imgui(
  ${imgui_SOURCE_DIR}
  ${imgui_BINARY_DIR}
  USE_STD_STRING 
  USE_FREETYPE 
)

function(add_spdlog SRC_DIR BIN_DIR)
  add_subdirectory(${SRC_DIR} ${BIN_DIR})
endfunction(add_spdlog)

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.12.0
  GIT_SHALLOW    ON
)
FetchContent_Populate(spdlog)
add_spdlog(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR})


function(add_eigen SRC_DIR BIN_DIR)
  set(EIGEN_TEST_CXX11 ON CACHE INTERNAL "")
  set(EIGEN_BUILD_DOC OFF CACHE INTERNAL "")
  set(BUILD_TESTING OFF CACHE INTERNAL "")
  add_subdirectory(${SRC_DIR} ${BIN_DIR})
endfunction(add_eigen)

FetchContent_Declare(
  eigen
  GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
  GIT_TAG        764b132a79aaf33e958169a14a011b2e0eec1400
  GIT_SHALLOW    ON
)
FetchContent_Populate(eigen)
add_eigen(${eigen_SOURCE_DIR} ${eigen_BINARY_DIR})

function(add_json SRC_DIR BIN_DIR)
  set(JSON_BuildTests OFF CACHE INTERNAL "")
  add_subdirectory(${SRC_DIR} ${BIN_DIR})
endfunction(add_json)

FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG        v3.11.2
  GIT_SHALLOW    ON
)
FetchContent_Populate(json)
add_json(${json_SOURCE_DIR} ${json_BINARY_DIR})

function(add_nfd SRC_DIR BIN_DIR)
  set(NFD_BUILD_TESTS OFF CACHE INTERNAL "")
  set(BUILD_SHARED_LIBS OFF)
  add_subdirectory(${SRC_DIR} ${BIN_DIR})
  add_library(nfd::nfd ALIAS nfd)
endfunction(add_nfd)

FetchContent_Declare(
  nfd_ext
  GIT_REPOSITORY https://github.com/btzy/nativefiledialog-extended.git
  GIT_TAG        v1.1.0
  GIT_SHALLOW    ON
)
FetchContent_Populate(nfd_ext)
add_nfd(${nfd_ext_SOURCE_DIR} ${nfd_ext_BINARY_DIR})

FetchContent_Declare(
  pcg_cpp
  GIT_REPOSITORY https://github.com/imneme/pcg-cpp.git
  GIT_TAG        428802d1a5634f96bcd0705fab379ff0113bcf13
  GIT_SHALLOW    ON
)
FetchContent_Populate(pcg_cpp)
add_pcg_cpp(${pcg_cpp_SOURCE_DIR} ${pcg_cpp_BINARY_DIR})


function(add_Imath SRC_DIR BIN_DIR)
  set(PYTHON OFF CACHE INTERNAL "")
  set(BUILD_DOCS OFF CACHE INTERNAL "")
  add_subdirectory(${SRC_DIR} ${BIN_DIR})
endfunction(add_Imath)

FetchContent_Declare(
  Imath
  GIT_REPOSITORY https://github.com/AcademySoftwareFoundation/Imath.git
  GIT_TAG        v3.1.9
  GIT_SHALLOW    ON
)
FetchContent_Populate(Imath)
add_Imath(${imath_SOURCE_DIR} ${imath_BINARY_DIR})

function(add_zlib SRC_DIR BIN_DIR)
  add_subdirectory(${SRC_DIR} ${BIN_DIR})
  target_include_directories(zlibstatic PUBLIC ${zlib_BINARY_DIR} ${zlib_SOURCE_DIR})
  add_library(ZLIB::ZLIB ALIAS zlibstatic)
endfunction(add_zlib)

FetchContent_Declare(
  zlib
  GIT_REPOSITORY https://github.com/madler/zlib.git
  GIT_TAG        v1.3
  GIT_SHALLOW    ON
)
FetchContent_Populate(zlib)
add_zlib(${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})

function(add_openexr SRC_DIR BIN_DIR)
  set(OPENEXR_INSTALL OFF CACHE INTERNAL "")
  set(OPENEXR_INSTALL_TOOLS OFF CACHE INTERNAL "")
  set(OPENEXR_INSTALL_EXAMPLES OFF CACHE INTERNAL "")
  set(OPENEXR_BUILD_TOOLS OFF CACHE INTERNAL "")
  add_subdirectory(${SRC_DIR} ${BIN_DIR})
endfunction(add_openexr)

FetchContent_Declare(
  openexr
  GIT_REPOSITORY https://github.com/AcademySoftwareFoundation/openexr.git
  GIT_TAG        v3.1.9
  GIT_SHALLOW    ON
)
FetchContent_Populate(openexr)
add_openexr(${openexr_SOURCE_DIR} ${openexr_BINARY_DIR})

function(add_oneTBB SRC_DIR BIN_DIR)
  set(BUILD_SHARED_LIBS ON CACHE INTERNAL "")
  set(TBB_TEST OFF CACHE INTERNAL "")
  add_subdirectory(${SRC_DIR} ${BIN_DIR})
endfunction(add_oneTBB)

FetchContent_Declare(
  oneTBB
  GIT_REPOSITORY https://github.com/oneapi-src/oneTBB.git
  GIT_TAG        v2021.10.0
  GIT_SHALLOW    ON
)
FetchContent_Populate(oneTBB)
add_oneTBB(${onetbb_SOURCE_DIR} ${onetbb_BINARY_DIR})
