add_library(CSRDirectLoader csr_direct_loader.c)
add_library(SCPA_MM_Iterators scpa_mm_iterators.c)

target_include_directories(CSRDirectLoader PRIVATE ../include)
target_include_directories(SCPA_MM_Iterators PRIVATE ../include)
target_link_libraries(SCPA_MM_Iterators MatrixMarketIO )
target_link_libraries(CSRDirectLoader SCPA_MM_Iterators)