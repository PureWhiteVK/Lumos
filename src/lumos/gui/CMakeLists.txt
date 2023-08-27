add_library(lumos_gui STATIC
    src/common.cpp
    src/context.cpp
    src/imgui_utils.cpp
    src/texture.cpp
    src/shader.cpp
    src/framebuffer.cpp
)
add_library(lumos::lumos_gui ALIAS lumos_gui)
set_property(TARGET lumos_gui PROPERTY CXX_STANDARD 17)
set_property(TARGET lumos_gui PROPERTY CXX_STANDARD_REQUIRED ON)
target_include_directories(lumos_gui PUBLIC include)
target_link_libraries(lumos_gui PUBLIC 
    imgui::imgui glad::glad glfw::glfw nfd::nfd
    lumos::lumos_core
)