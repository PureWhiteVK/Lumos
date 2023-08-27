function(add_imgui SRC_DIR BIN_DIR)
  set(options USE_STD_STRING USE_FREETYPE)
  set(one_value_args "")
  set(multi_value_args "")
  cmake_parse_arguments(ARGS "${options}" "${one_value_args}"
                             "${multi_value_args}" ${ARGN} )

  message(STATUS "add imgui (opengl3 + glfw) [ STATIC ] with c++17")


  list(APPEND CMAKE_MESSAGE_INDENT "  ")

  add_library(imgui STATIC
    ${SRC_DIR}/imgui_demo.cpp 
    ${SRC_DIR}/imgui_draw.cpp 
    ${SRC_DIR}/imgui_tables.cpp 
    ${SRC_DIR}/imgui_widgets.cpp 
    ${SRC_DIR}/imgui.cpp 
  )

  add_library(imgui::imgui ALIAS imgui)

  set_property(TARGET imgui PROPERTY RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
  set_property(TARGET imgui PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${BIN_DIR})
  set_property(TARGET imgui PROPERTY LIBRARY_OUTPUT_DIRECTORY ${BIN_DIR})
  set_property(TARGET imgui PROPERTY PDB_OUTPUT_DIRECTORY     ${BIN_DIR})

  set_property(TARGET imgui PROPERTY CXX_STANDARD 17)
  set_property(TARGET imgui PROPERTY CXX_STANDARD_REQUIRED ON)

  target_include_directories(imgui PUBLIC 
    ${SRC_DIR}
  )
  # backends
  target_sources(imgui PUBLIC     
    ${SRC_DIR}/backends/imgui_impl_opengl3.cpp
    ${SRC_DIR}/backends/imgui_impl_glfw.cpp
  )

  target_include_directories(imgui PUBLIC 
    ${SRC_DIR}/backends
  )

  target_link_libraries(imgui PUBLIC 
    glfw::glfw
  )
  
  # extensions
  if ( ARGS_USE_STD_STRING )
    message(STATUS "enable std::string support for imgui::InputText")
    target_sources(imgui PUBLIC 
      ${SRC_DIR}/misc/cpp/imgui_stdlib.cpp
    )
    target_include_directories(imgui PUBLIC 
      ${SRC_DIR}/misc/cpp
    )
  endif()
  
  if ( ARGS_USE_FREETYPE )
    find_package(Freetype)
    if(Freetype_FOUND)
      message(STATUS "enable freetype for imgui")
      target_sources(imgui PRIVATE 
        ${SRC_DIR}/misc/freetype/imgui_freetype.cpp
      )
      target_include_directories(imgui PRIVATE 
        ${SRC_DIR}/misc/freetype
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