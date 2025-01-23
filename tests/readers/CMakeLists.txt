
add_executable(TestNormalMMReader testNormalMMreader.c)
configure_file(resources/matrix.mm resources/matrix.mm)

target_link_libraries(TestNormalMMReader CSRDirectLoader)
add_test(TestNormalMMReader TestNormalMMReader)

add_executable(TestSymmetricMMReader testSymmetricMMreader.c)
configure_file(resources/symm_matrix.mm resources/symm_matrix.mm)

target_link_libraries(TestSymmetricMMReader CSRDirectLoader)
add_test(TestSymmetricMMReader TestSymmetricMMReader)


add_executable(TestSkewMMReader testSkewMMreader.c)
configure_file(resources/skew_matrix.mm resources/skew_matrix.mm)

target_link_libraries(TestSkewMMReader CSRDirectLoader)
add_test(TestSkewMMReader TestSkewMMReader)

add_executable(TestPatternMMReader testPatternMMreader.c)
configure_file(resources/pattern_matrix.mm resources/pattern_matrix.mm)

target_link_libraries(TestPatternMMReader CSRDirectLoader)
add_test(TestPatternMMReader TestPatternMMReader)
