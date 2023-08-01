add_library(stb_image src/stb_image.c)
target_include_directories(stb_image PUBLIC include)
add_library(stb::stb_image ALIAS stb_image)

add_library(stb_image_write src/stb_image_write.c)
target_include_directories(stb_image_write PUBLIC include)
add_library(stb::stb_image_write ALIAS stb_image_write)