#include <stdio.h>
#include <math.h>

#include "matrix_market_loaders/mm_loader.h"


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
    SCPA_MMLOADER_HLL_LOADER_DATA loader ;
    if (file == NULL) {
        perror("Error opening file: ") ;
        return -1 ;
    }

    if (SCPA_HLL_DIRECT_LOADER_Init(file, &loader, 1)) {
        fclose(file) ;
        return -1 ;
    }

    HLL_LOADER_DATA *cast = loader.data ;

    int retval = 0 ;

    if (cast->rows != correct.rows) {
        retval = -1 ;
        goto terminate ;
    }
    if (cast->cols != correct.cols) {
        retval = -1 ;
        goto terminate ;
    }
    if (cast->nzs != correct.nzs) {
        retval = -1 ;
        goto terminate ;
    }
    if (cast->hack_size != correct.hack_size) {
        retval = -1 ;
        goto terminate ;
    }

    for (int i = 0; i < 7 ; i++) {
        if (cast->ellpacks[i].rows != correct.ellpacks[i].rows) {
            retval = -1 ;
            goto terminate ;
        }
        if (cast->ellpacks[i].cols != correct.ellpacks[i].cols) {
            retval = -1 ;
            goto terminate ;
        }
        if (cast->ellpacks[i].maxnz != correct.ellpacks[i].maxnz) {
            retval = -1 ;
            goto terminate ;
        }

        for (int j = 0; j < cast->ellpacks[i].maxnz * cast->ellpacks[i].rows ; j++) {
            if (cast->ellpacks[i].columnMat[j] != correct.ellpacks[i].columnMat[j]) {
                retval = -1 ;
                goto terminate ;
            }
            if (fabs(cast->ellpacks[i].nzMat[j] - correct.ellpacks[i].nzMat[j]) > 0.01) {
                retval = -1 ;
                goto terminate ;
            }
        }
    }

terminate :

    fclose(file) ;
    SCPA_HLL_DIRECT_LOADER_Destroy(&loader) ;

    return retval ;
}