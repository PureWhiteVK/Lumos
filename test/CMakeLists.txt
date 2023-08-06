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
  TARGET_NAME test_stb_app
  SOURCES test_stb.cpp
  DEPENDENCIES stb::stb_image stb::stb_image_write 
               spdlog::spdlog
)

add_testapp(
  ENABLE_TEST 
  TARGET_NAME test_pcg
  SOURCES test_pcg.cpp
  DEPENDENCIES pcg::pcg-cpp
)

add_testapp(
  ENABLE_TEST 
  TARGET_NAME test_eigen_image
  SOURCES test_eigen_image.cpp
  DEPENDENCIES lumos::lumos_core
)

add_testapp(
  TARGET_NAME test_openexr
  SOURCES test_openexr.cpp
  DEPENDENCIES lumos::lumos_core
)

add_testapp(
  TARGET_NAME test_viewer
  SOURCES test_viewer.cpp
  DEPENDENCIES imgui::imgui glad::glad glfw::glfw 
               lumos::lumos_core nfd::nfd
)

add_testapp(
  TARGET_NAME test_random_permutation
  SOURCES test_random_permutation.cpp
  DEPENDENCIES imgui::imgui glad::glad glfw::glfw 
               lumos::lumos_core nfd::nfd pcg::pcg-cpp
)