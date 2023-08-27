add_library(lumos_core STATIC
  src/common.cpp
  src/color.cpp
  src/imageio.cpp
)
add_library(lumos::lumos_core ALIAS lumos_core)
set_property(TARGET lumos_core PROPERTY CXX_STANDARD 17)
set_property(TARGET lumos_core PROPERTY CXX_STANDARD_REQUIRED ON)
target_compile_definitions(lumos_core PRIVATE DATA_PATH="${DATA_PATH}")
target_include_directories(lumos_core PUBLIC include)
target_link_libraries(lumos_core PUBLIC 
  OpenEXR::OpenEXR 
  spdlog::spdlog 
  Eigen3::Eigen 
  stb::stb_image_write 
  stb::stb_image 
  nlohmann_json::nlohmann_json 
)