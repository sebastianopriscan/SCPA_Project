
configure_file(resources/pattern_matrix.mm resources/pattern_matrix.mm)
configure_file(resources/test_vector.vec resources/test_vector.vec)

add_executable(TestSerialCSRMultiplier test_serial_multiplier.c)

target_link_libraries(TestSerialCSRMultiplier CSRDirectLoader SCPA_Serial_Kernel SCPAVectorLoaders)
add_test(TestSerialCSRMultiplier TestSerialCSRMultiplier)

foreach(SCPA_CSR_OMP_KERNEL_VERSION ${SCPA_CSR_OMP_KERNEL_VERSIONS})

    set(SCPA_CSR_OMP_LIB "SCPA_CSR_Omp_Kernel_v${SCPA_CSR_OMP_KERNEL_VERSION}")
    set(SCPA_CSR_OMP_TEST_MULTIPLIER "TestCompareOmpCSRMultiplierv${SCPA_CSR_OMP_KERNEL_VERSION}")

    add_executable(${SCPA_CSR_OMP_TEST_MULTIPLIER} compare_omp_csr.c)

    if(DEFINED CACHE{MATRIX_CACHE_DIR})
        target_compile_definitions(${SCPA_CSR_OMP_TEST_MULTIPLIER} PRIVATE "MATRIX_CACHE_DIR=${MATRIX_CACHE_DIR}")
    endif(DEFINED CACHE{MATRIX_CACHE_DIR})
    

    target_link_libraries(${SCPA_CSR_OMP_TEST_MULTIPLIER} CSRDirectLoader ${SCPA_CSR_OMP_LIB} SparseMatrixCache)
    add_test(${SCPA_CSR_OMP_TEST_MULTIPLIER} ${SCPA_CSR_OMP_TEST_MULTIPLIER})

endforeach(SCPA_CSR_OMP_KERNEL_VERSION ${SCPA_CSR_OMP_KERNEL_VERSIONS})

add_executable(SCPAGenerateComparations generate_comparations.c)
target_link_libraries(SCPAGenerateComparations SCPA_Serial_Kernel CSRDirectLoader SparseMatrixCache)
configure_file(resources/comparative/matrices resources/comparative/matrices)

add_custom_target(symlink_comparations ALL
                  COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/resources/comparative/results resources/comparative/results)