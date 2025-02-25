#ifndef SCPA_MMLOADER_H
#define SCPA_MMLOADER_H

#include <stdio.h>
#include "common/definitions.h"

#define define_new_loader_type(TYPE)                                                                    \
    typedef struct _SCPA_MMLOADER_##TYPE {                                                              \
                                                                                                        \
        TYPE *data ;                                                                                    \
                                                                                                        \
        double (*SCPA_MMLOADER_ReadAt)(IN struct _SCPA_MMLOADER_##TYPE *, IN int row, IN int col) ;     \
                                                                                                        \
    } SCPA_MMLOADER_##TYPE                                                                              \


struct _CSR_LOADER_DATA {
    int *rowIdxs ;
    int *colIdxs ;
    double *nzs ;
    int rows, cols, nzNum ;
} ;
typedef struct _CSR_LOADER_DATA CSR_LOADER_DATA ;


struct _ELL_LOADER_DATA {
    int rows ;
    int cols ;
    int maxnz ;
    int *columnMat ;
    double *nzMat ;
} ;
typedef struct _ELL_LOADER_DATA ELL_LOADER_DATA ;

struct _HLL_LOADER_DATA {
    int hack_size ;
    int rows ;
    int cols ;
    int nzs ;
    ELL_LOADER_DATA *ellpacks ;
} ;
typedef struct _HLL_LOADER_DATA HLL_LOADER_DATA ;

define_new_loader_type(CSR_LOADER_DATA) ;
define_new_loader_type(HLL_LOADER_DATA) ;

int SCPA_CSR_DIRECT_LOADER_Init(IN FILE *file, OUT SCPA_MMLOADER_CSR_LOADER_DATA *out) ;
int SCPA_CSR_DIRECT_LOADER_Destroy(IN SCPA_MMLOADER_CSR_LOADER_DATA *loader) ;

int SCPA_HLL_DIRECT_LOADER_Init(IN FILE *file, OUT SCPA_MMLOADER_HLL_LOADER_DATA *out, int hackSize) ;
int SCPA_HLL_DIRECT_LOADER_Destroy(IN SCPA_MMLOADER_HLL_LOADER_DATA *loader) ;

#endif