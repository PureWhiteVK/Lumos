function(add_imgui)
  set(options DYNAMIC ARGS_USE_STD_STRING USE_FREETYPE)
  set(one_value_args IMGUI_DIR)
  set(multi_value_args "")
  cmake_parse_arguments(ARGS "${options}" "${one_value_args}"
                             "${multi_value_args}" ${ARGN} )
  set(build_type STATIC)
  if ( ARGS_DYNAMIC ) 
    set(build_type DYNAMIC)
  endif()

  message(STATUS "add imgui (opengl3 + glfw) [ ${build_type} ]")

  list(APPEND CMAKE_MESSAGE_INDENT "  ")

  add_library(imgui ${build_type} 
    ${ARGS_IMGUI_DIR}/imgui_demo.cpp 
    ${ARGS_IMGUI_DIR}/imgui_draw.cpp 
    ${ARGS_IMGUI_DIR}/imgui_tables.cpp 
    ${ARGS_IMGUI_DIR}/imgui_widgets.cpp 
    ${ARGS_IMGUI_DIR}/imgui.cpp 
  )

  add_library(imgui::imgui ALIAS imgui)

  set_property(TARGET imgui PROPERTY CXX_STANDARD 17)
  set_property(TARGET imgui PROPERTY CXX_STANDARD_REQUIRED ON)

  target_include_directories(imgui PUBLIC 
    ${ARGS_IMGUI_DIR}
  )
  # backends
  target_sources(imgui PUBLIC     
    ${ARGS_IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    ${ARGS_IMGUI_DIR}/backends/imgui_impl_glfw.cpp
  )

  target_include_directories(imgui PUBLIC 
    ${ARGS_IMGUI_DIR}/backends
  )

  target_link_libraries(imgui PUBLIC 
    glfw::glfw
  )
  
  # extensions
  if ( ARGS_ARGS_USE_STD_STRING )
    message(STATUS "enable std::string support for imgui::InputText")
    target_sources(imgui PUBLIC 
      ${ARGS_IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp
    )
    target_include_directories(imgui PUBLIC 
      ${ARGS_IMGUI_DIR}/misc/cpp
    )
  endif()
  
  if ( ARGS_USE_FREETYPE )
    find_package(Freetype)
    if(Freetype_FOUND)
      message(STATUS "enable freetype for imgui")
      target_sources(imgui PRIVATE 
        ${ARGS_IMGUI_DIR}/misc/freetype/imgui_freetype.cpp
      )
      target_include_directories(imgui PRIVATE 
        ${ARGS_IMGUI_DIR}/misc/freetype
      )
      target_link_libraries(imgui PRIVATE 
        Freetype::Freetype
      )
      target_compile_definitions(imgui PRIVATE 
        IMGUI_ENABLE_FREETYPE
      )
    else()
    message(STATUS "disable freetype for imgui")
    endif()
  endif()
  
  list(POP_BACK CMAKE_MESSAGE_INDENT)

endfunction(add_imgui)