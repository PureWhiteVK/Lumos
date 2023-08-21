add_library(stb_image STATIC src/stb_image.c)
target_include_directories(stb_image PUBLIC include)
add_library(stb::stb_image ALIAS stb_image)

add_library(stb_image_write STATIC src/stb_image_write.c)
target_include_directories(stb_image_write PUBLIC include)
# if(WIN32)
#     target_compile_definitions(stb_image_write PRIVATE -DSTBIW_WINDOWS_UTF8)
# endif()
add_library(stb::stb_image_write ALIAS stb_image_write)

# add_library(stb_image INTERFACE)
# target_include_directories(stb_image INTERFACE include)
# add_library(stb::stb_image ALIAS stb_image)

# add_library(stb_image_write INTERFACE)
# target_include_directories(stb_image_write INTERFACE include)
# add_library(stb::stb_image_write ALIAS stb_image_write)