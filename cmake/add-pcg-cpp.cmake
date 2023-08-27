function(add_pcg_cpp SRC_DIR BIN_DIR)
  message(STATUS "add pcg_cpp [ INTERFACE ] with c++11")

  add_library(pcg_cpp INTERFACE)
  target_include_directories(pcg_cpp INTERFACE ${SRC_DIR}/include)

  set_property(TARGET pcg_cpp PROPERTY RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
  set_property(TARGET pcg_cpp PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${BIN_DIR})
  set_property(TARGET pcg_cpp PROPERTY LIBRARY_OUTPUT_DIRECTORY ${BIN_DIR})
  set_property(TARGET pcg_cpp PROPERTY PDB_OUTPUT_DIRECTORY     ${BIN_DIR})

  set_property(TARGET pcg_cpp PROPERTY CXX_STANDARD 11)
  set_property(TARGET pcg_cpp PROPERTY CXX_STANDARD_REQUIRED ON)

  add_library(pcg::pcg_cpp ALIAS pcg_cpp)

endfunction()