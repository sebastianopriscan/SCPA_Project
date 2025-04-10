add_library(SCPA_Args_Parser parse_args.c)
target_link_libraries(SCPA_Args_Parser
    CSRDirectLoader 
    HLLDirectLoader 
    SparseMatrixCache
    SCPAVectorLoaders
)