macro(add_testapp)
  set(options ENABLE_TEST)
  set(one_value_args TARGET_NAME)
  set(multi_value_args SOURCES DEPENDENCIES)
  cmake_parse_arguments(ARG "${options}" "${one_value_args}"
                             "${multi_value_args}" ${ARGN} )
  message(STATUS "add testapp [ ${ARG_TARGET_NAME} ]")
  add_executable(${ARG_TARGET_NAME}
    ${ARG_SOURCES}
  )
  set_property(TARGET ${ARG_TARGET_NAME} PROPERTY CXX_STANDARD 17)
  set_property(TARGET ${ARG_TARGET_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
  target_link_libraries(${ARG_TARGET_NAME} PRIVATE 
    ${ARG_DEPENDENCIES}
  )
  # only need on windows，但是还需要拷贝 debug 信息
  add_custom_command(TARGET ${ARG_TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:${ARG_TARGET_NAME}> $<TARGET_RUNTIME_DLLS:${ARG_TARGET_NAME}>
    COMMAND_EXPAND_LISTS
  )
  
  if(ARG_ENABLE_TEST)
    add_test(NAME TESTAPP_${ARG_TARGET_NAME} COMMAND ${ARG_TARGET_NAME})
  endif()
endmacro(add_testapp)