add_executable(TestLoadFromFile testLoadFromFile.c)
target_link_libraries(TestLoadFromFile SCPAVectorLoaders)
add_test(TestLoadFromFile TestLoadFromFile)

configure_file(resources/testVector.vec resources/testVector.vec)