set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "")
set(GLFW_INSTALL OFF CACHE INTERNAL "")
add_subdirectory(glfw)
add_library(glfw::glfw ALIAS glfw)

add_imgui(
  USE_STD_STRING 
  USE_FREETYPE 
  IMGUI_DIR imgui
)

set(SPDLOG_ENABLE_PCH ON CACHE INTERNAL "")
add_subdirectory(spdlog)

# the INTERNAL just disable cmake-gui showing this
# disable eigen doc (so many examples!)
set(EIGEN_TEST_CXX11 ON CACHE INTERNAL "")
set(EIGEN_BUILD_DOC OFF CACHE INTERNAL "")
set(BUILD_TESTING OFF CACHE INTERNAL "")
add_subdirectory(eigen)

# disable json tests
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(json)

# nativefiledialog-extended
set(NFD_BUILD_TESTS OFF CACHE INTERNAL "")
add_subdirectory(nativefiledialog-extended)
add_library(nfd::nfd ALIAS nfd)

add_pcg_cpp(PCG_CPP_DIR pcg-cpp)

set(OPENEXR_INSTALL OFF CACHE INTERNAL "")
set(OPENEXR_INSTALL_TOOLS OFF CACHE INTERNAL "")
set(OPENEXR_INSTALL_EXAMPLES OFF CACHE INTERNAL "")
set(OPENEXR_BUILD_TOOLS OFF CACHE INTERNAL "")
add_subdirectory(openexr)

set(BUILD_SHARED_LIBS ON CACHE INTERNAL "")
set(TBB_TEST OFF CACHE INTERNAL "")
add_subdirectory(oneTBB)