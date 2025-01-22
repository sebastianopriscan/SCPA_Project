
add_executable(TestNormalMMReader testNormalMMreader.c)
add_custom_command(TARGET TestNormalMMReader POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/tests/readers/resources/ $<TARGET_FILE_DIR:TestNormalMMReader>/resources/)

target_link_libraries(TestNormalMMReader CSRDirectLoader)
add_test(TestNormalMMReader TestNormalMMReader)

add_executable(TestSymmetricMMReader testSymmetricMMreader.c)

target_link_libraries(TestSymmetricMMReader CSRDirectLoader)
add_test(TestSymmetricMMReader TestSymmetricMMReader)


add_executable(TestSkewMMReader testSkewMMreader.c)

target_link_libraries(TestSkewMMReader CSRDirectLoader)
add_test(TestSkewMMReader TestSkewMMReader)