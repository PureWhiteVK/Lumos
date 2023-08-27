add_compile_definitions(DATA_PATH="${DATA_PATH}")

add_testapp(
  TARGET_NAME test_imgui_app
  SOURCES test_imgui.cpp
  DEPENDENCIES stb::stb_image stb::stb_image_write 
               spdlog::spdlog imgui::imgui 
               glad::glad glfw::glfw
)

add_testapp(
  ENABLE_TEST 
  TARGET_NAME test_pcg
  SOURCES test_pcg.cpp
  DEPENDENCIES pcg::pcg_cpp
)

add_testapp(
  TARGET_NAME test_imageio
  SOURCES test_imageio.cpp
  DEPENDENCIES lumos::lumos_core
)

add_testapp(
  TARGET_NAME test_viewer
  SOURCES test_viewer.cpp
  DEPENDENCIES lumos::lumos_gui nfd::nfd
)

add_testapp(
  TARGET_NAME test_random_permutation
  SOURCES test_random_permutation.cpp
  DEPENDENCIES lumos::lumos_core 
               pcg::pcg_cpp
)

include(FetchContent)
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG        47585fde0c49fa77a2bf2fb1d2ead06999fd4b6e
  GIT_SHALLOW    ON
)
FetchContent_Populate(glm)
add_subdirectory(${glm_SOURCE_DIR} ${glm_BINARY_DIR})

add_testapp(
  TARGET_NAME test_gl
  SOURCES test_gl.cpp
  DEPENDENCIES lumos::lumos_core 
               lumos::lumos_gui
               glm::glm
)