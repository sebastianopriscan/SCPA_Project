add_subdirectory(serial/csr)
add_subdirectory(parallel/omp/csr)

add_executable(SCPA_Serial_Multiplier serialMultiply.c)
target_link_libraries(SCPA_Serial_Multiplier SCPA_Serial_Kernel SCPA_Args_Parser)

foreach(SCPA_CSR_OMP_KERNEL_VERSION ${SCPA_CSR_OMP_KERNEL_VERSIONS})

    set(SCPA_CSR_OMP_LIB "SCPA_CSR_Omp_Kernel_v${SCPA_CSR_OMP_KERNEL_VERSION}")
    set(SCPA_CSR_OMP_MULTIPLIER "SCPA_CSR_OMP_Multiplier_v${SCPA_CSR_OMP_KERNEL_VERSION}")

    add_executable(${SCPA_CSR_OMP_MULTIPLIER} ompMultiply.c)
    target_link_libraries(${SCPA_CSR_OMP_MULTIPLIER} ${SCPA_CSR_OMP_LIB} SCPA_Args_Parser)

endforeach(SCPA_CSR_OMP_KERNEL_VERSION ${SCPA_CSR_OMP_KERNEL_VERSIONS})

configure_file(results_to_query.awk results_to_query.awk FILE_PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE)