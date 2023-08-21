set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
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
  DEPENDENCIES pcg::pcg-cpp
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
               nfd::nfd pcg::pcg-cpp
)

find_package(glm REQUIRED)

if(glm_FOUND)
add_testapp(
  TARGET_NAME test_gl
  SOURCES test_gl.cpp
  DEPENDENCIES lumos::lumos_core 
               lumos::lumos_gui
               glm::glm
)
endif()