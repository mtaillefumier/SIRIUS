set(_tests "test_hdf5;test_allgather;\
read_atom;test_mdarray;test_xc;test_hloc;\
test_mpi_grid;test_enu;test_eigen;test_gemm;test_gemm2;test_wf_inner_v3;test_memop;\
test_mem_pool;test_mem_alloc;test_examples;test_wf_inner_v4;test_bcast_v2;test_p2p_cyclic;\
test_wf_ortho_6;test_mixer;test_davidson;test_lapw_xc;test_phase;test_bessel;test_fp;test_pppw_xc;\
test_exc_vxc")

foreach(_test ${_tests})
  add_executable(${_test} ${_test}.cpp)
  target_link_libraries(${_test} PRIVATE sirius)
  install(TARGETS ${_test} RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
endforeach()

if(CREATE_FORTRAN_BINDINGS)
  add_executable(test_fortran_api test_fortran_api.f90)
  target_link_libraries(test_fortran_api PRIVATE sirius)
  install(TARGETS test_fortran_api RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
endif(CREATE_FORTRAN_BINDINGS)
