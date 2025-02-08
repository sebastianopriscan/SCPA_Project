#include <stdio.h>
#include <math.h>

#include "matrix_market_loaders/mm_loader.h"

#define as_impl_data(data) (HLL_LOADER_DATA *)(data)

struct _ELL_LOADER_DATA {
    int rows ;
    int cols ;
    int maxnz ;
    int *columnMat ;
    double *nzMat ;
} ;
typedef struct _ELL_LOADER_DATA ELL_LOADER_DATA ;

#define ELL_LOADER_DATA_SIZE(rows, nz) (sizeof(ELL_LOADER_DATA) + sizeof(int) * rows * nz + sizeof(double) * rows * nz)

struct _HLL_LOADER_DATA {
    int hack_size ;
    int rows ;
    int cols ;
    int nzs ;
    ELL_LOADER_DATA *ellpacks ;
} ;
typedef struct _HLL_LOADER_DATA HLL_LOADER_DATA ;

ELL_LOADER_DATA ellpacks[7] = {
    {
        .rows = 1,
        .cols = 7,
        .maxnz = 1,
        .columnMat = (int []){4},
        .nzMat = (double []){0.433},
    },
    {
        .rows = 1,
        .cols = 7,
        .maxnz = 1,
        .columnMat = (int []){5},
        .nzMat = (double []){2.16},
    },
    {
        .rows = 1,
        .cols = 7,
        .maxnz = 0,
        .columnMat = (int []){0},
        .nzMat = (double []){0.},
    },
    {
        .rows = 1,
        .cols = 7,
        .maxnz = 1,
        .columnMat = (int []){6},
        .nzMat = (double []){-8.43},
    },
    {
        .rows = 1,
        .cols = 7,
        .maxnz = 1,
        .columnMat = (int []){0},
        .nzMat = (double []){-0.433},
    },
    {
        .rows = 1,
        .cols = 7,
        .maxnz = 1,
        .columnMat = (int []){1},
        .nzMat = (double []){-2.16},
    },
    {
        .rows = 1,
        .cols = 7,
        .maxnz = 1,
        .columnMat = (int []){3},
        .nzMat = (double []){8.43},
    },
} ;

HLL_LOADER_DATA correct = {
    .hack_size = 1,
    .rows = 7,
    .cols = 7,
    .nzs = 6,
    .ellpacks = ellpacks
} ;

char buf[4096] ;

int main(void) {
    FILE *file = fopen("./resources/skew_matrix.mm", "r") ;
    SCPA_MMLOADER loader ;
    if (file == NULL) {
        perror("Error opening file: ") ;
        return -1 ;
    }

    if (SCPA_HLL_DIRECT_LOADER_Init(file, &loader, 1)) {
        return -1 ;
    }

    HLL_LOADER_DATA *cast = as_impl_data(loader.data) ;

    if (cast->rows != correct.rows) return -1 ;
    if (cast->cols != correct.cols) return -1 ;
    if (cast->nzs != correct.nzs) return -1 ;
    if (cast->hack_size != correct.hack_size) return -1 ;

    for (int i = 0; i < 7 ; i++) {
        if (cast->ellpacks[i].rows != correct.ellpacks[i].rows) return -1 ;
        if (cast->ellpacks[i].cols != correct.ellpacks[i].cols) return -1 ;
        if (cast->ellpacks[i].maxnz != correct.ellpacks[i].maxnz) return -1 ;

        for (int j = 0; j < cast->ellpacks[i].maxnz * cast->ellpacks[i].rows ; j++) {
            if (cast->ellpacks[i].columnMat[j] != correct.ellpacks[i].columnMat[j]) return -1 ;
            if (fabs(cast->ellpacks[i].nzMat[j] - correct.ellpacks[i].nzMat[j]) > 0.01) return -1 ;
        }
    }

    return 0 ;
}