
configure_file(resources/pattern_matrix.mm resources/pattern_matrix.mm)
configure_file(resources/test_vector.vec resources/test_vector.vec)

add_executable(TestSerialCSRMultiplier test_serial_multiplier.c)

target_link_libraries(TestSerialCSRMultiplier CSRDirectLoader SCPA_Serial_Kernel SCPAVectorLoaders)
add_test(TestSerialCSRMultiplier TestSerialCSRMultiplier)

add_executable(SCPAGenerateComparations generate_comparations.c)
target_link_libraries(SCPAGenerateComparations SCPA_Serial_Kernel CSRDirectLoader SparseMatrixCache)
configure_file(resources/comparative/matrices resources/comparative/matrices)

add_custom_target(symlink_comparations ALL
                  COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/resources/comparative/results resources/comparative/results)