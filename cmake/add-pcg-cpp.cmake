function(add_pcg_cpp)
  set(options DYNAMIC)
  set(one_value_args PCG_CPP_DIR)
  set(multi_value_args "")
  cmake_parse_arguments(ARGS "${options}" "${one_value_args}"
                             "${multi_value_args}" ${ARGN} )
  set(build_type STATIC)
  if ( ARGS_DYNAMIC ) 
    set(build_type DYNAMIC)
  endif()

  message(STATUS "add pcg-cpp [ ${build_type} ]")

  add_library(pcg-cpp INTERFACE)
  target_include_directories(pcg-cpp INTERFACE ${ARGS_PCG_CPP_DIR}/include)
  add_library(pcg::pcg-cpp ALIAS pcg-cpp)
endfunction()