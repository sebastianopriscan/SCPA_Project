#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "args/parse_args.h"
#include "matrix_market_loaders/mm_loader.h"
#include "sparse_matrix_cache/sparse_matrix_cache.h"
#include "vector_loaders/vector_loaders.h"

typedef enum _SCPA_VectorType {
    VECTOR_FILE,
    VECTOR_RANDOM,
    VECTOR_RANDOM_PATTERN
} SCPA_VectorType ;

const char *SCPA_DIRECT_HLL_CLASS_STR = "SCPA_DIRECT_HLL_CLASS";
const char *SCPA_DIRECT_CSR_CLASS_STR = "SCPA_DIRECT_CSR_CLASS";

const char *SCPA_LoaderClassToString(SCPA_LoaderClass class) {
    switch(class) {
        case (SCPA_DIRECT_CSR_CLASS) :
            return SCPA_DIRECT_CSR_CLASS_STR ;
        case (SCPA_DIRECT_HLL_CLASS) :
            return SCPA_DIRECT_HLL_CLASS_STR ;
        default :
            return NULL ;
    }
}

SCPA_ParsedArgs *SCPA_ParseArgs(int argc, char **argv) {

    if (argc < 5) goto error_return ;

    char *cacheDir = "/tmp/SCPA_Project" ;
    int hackSize = 32 ;
    int seed = 42 ;
    int vectorSize = 0 ;
    int times = 1;
    for (int i = 5 ; i < argc ; i++) {
        if (strcmp("--cache-dir", argv[i]) == 0 && i+1 < argc) {
            cacheDir = argv[i+1] ;
        }
        else if (strcmp("--hll-hacksize", argv[i]) == 0 && i+1 < argc) {
            char *endptr ;
            hackSize = strtol(argv[i+1], &endptr, 0) ;
            if (*endptr != '\0' || *argv[i+1] == '\0' || errno == ERANGE || errno == EINVAL) 
                goto error_return ;
        }
        else if (strcmp("--rand-seed", argv[i]) == 0 && i+1 < argc) {
            char *endptr ;
            seed = strtol(argv[i+1], &endptr, 0) ;
            if (*endptr != '\0' || *argv[i+1] == '\0' || errno == ERANGE || errno == EINVAL) 
                goto error_return ;
        }
        else if (strcmp("--times", argv[i]) == 0 && i+1 < argc) {
            char *endptr ;
            times = strtol(argv[i+1], &endptr, 0) ;
            if (*endptr != '\0' || *argv[i+1] == '\0' || errno == ERANGE || errno == EINVAL) 
                goto error_return ;
        }
    }

    SCPA_ParsedArgs *args = malloc(sizeof(SCPA_ParsedArgs)) ;

    if (args == NULL) goto error_return ;
    args->times = times ;

    FILE *file ;
    if (SCPA_SPCACHE_OpenMatrix(argv[2], argv[1], cacheDir, &file)) goto error_free_args ;

    if (strcmp(argv[3], "SCPA_DIRECT_CSR_CLASS") == 0) {
        args->loader = malloc(sizeof(SCPA_MMLOADER_CSR_LOADER_DATA)) ;
        if (args->loader == NULL) goto error_close_file ;
        if (SCPA_CSR_DIRECT_LOADER_Init(file, (args->loader))) goto error_free_and_close_file ;
        args->loaderClass = SCPA_DIRECT_CSR_CLASS ;
        vectorSize = ((CSR_LOADER_DATA *)((SCPA_MMLOADER_CSR_LOADER_DATA *)args->loader)->data)->rows ;
    }
    else if (strcmp(argv[3], "SCPA_DIRECT_HLL_CLASS") == 0) {
        args->loader = malloc(sizeof(SCPA_MMLOADER_HLL_LOADER_DATA)) ;
        if (args->loader == NULL) goto error_close_file ;
        if (SCPA_HLL_DIRECT_LOADER_Init(file, (args->loader), hackSize)) goto error_free_and_close_file ;
        args->loaderClass = SCPA_DIRECT_HLL_CLASS ;
        vectorSize = ((HLL_LOADER_DATA *)((SCPA_MMLOADER_HLL_LOADER_DATA *)args->loader)->data)->rows ;
    } else goto error_close_file ;

    if (strcmp(argv[4], "VECTOR_FILE") == 0 && argc >= 5) {
        if ((args->output = SCPA_VECTOR_LoadFromFile(argv[5])) == NULL) goto error_close_matrix ;
    }
    else if (strcmp(argv[4], "VECTOR_RANDOM") == 0) {
        if ((args->output = SCPA_VECTOR_LoadRandom(vectorSize, seed)) == NULL) goto error_close_matrix ;
    }
    else if (strcmp(argv[4], "VECTOR_RANDOM_PATTERN") == 0) {
        if ((args->output = SCPA_VECTOR_LoadRandomPattern(vectorSize, seed)) == NULL) goto error_close_matrix ;
    }
    else goto error_close_matrix ;

    fclose(file) ;
    return args ;

error_close_matrix :

    if (strcmp(argv[3], "SCPA_DIRECT_CSR_CLASS") == 0) {
        SCPA_CSR_DIRECT_LOADER_Destroy(args->loader) ;
    }
    else if (strcmp(argv[3], "SCPA_DIRECT_HLL_CLASS") == 0) {
        SCPA_HLL_DIRECT_LOADER_Destroy(args->loader) ;
    }

error_free_and_close_file :

    free(args->loader);

error_close_file :

    fclose(file) ;

error_free_args :

    free(args) ;

error_return :

    fprintf(stderr, "Usage: ./progname matrixGroup matrixName format{SCPA_DIRECT_(CSR|HLL)_CLASS} "
        "vectorRetrieval{VECTOR_(FILE|RANDOM(?:_PATTERN))} [vectorFile --cache-dir cacheDir --hll-hacksize hacksize "
        "--rand-seed seed --times times]\n") ;
    return NULL ;
}