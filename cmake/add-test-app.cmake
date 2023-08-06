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
  target_link_libraries(${ARG_TARGET_NAME} PRIVATE 
    ${ARG_DEPENDENCIES}
  )
  if(ARG_ENABLE_TEST)
    add_test(NAME TESTAPP_${ARG_TARGET_NAME} COMMAND ${ARG_TARGET_NAME})
  endif()
endmacro(add_testapp)