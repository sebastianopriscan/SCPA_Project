
add_executable(TestNormalMMReader testNormalMMreader.c)
add_custom_command(TARGET TestNormalMMReader POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/tests/readers/resources/ $<TARGET_FILE_DIR:TestNormalMMReader>/resources/)

target_link_libraries(TestNormalMMReader CSRDirectLoader)
add_test(TestNormalMMReader TestNormalMMReader)