#include <stdio.h>
#include <math.h>

#include "matrix_market_loaders/mm_loader.h"


int correctColumnMat1[14] = {2, 6,
                            0, 0,
                            0, 2, 
                            5, 0, 
                            0, 0,
                            3, 5,
                            0,6} ;

double correctNzMat1[14] = {10.5, 3.0,
                            0., 0.,
                            10.5, -0.145,
                            7.9, 0,
                            0., 0.,
                            7.9, 2.0,
                            3.0, 0.433} ;

ELL_LOADER_DATA ellpacks[1] = {
    {
        .rows = 7,
        .cols = 7,
        .maxnz = 2,
        .columnMat = correctColumnMat1,
        .nzMat = correctNzMat1,
    },
} ;

HLL_LOADER_DATA correct = {
    .hack_size = 7,
    .rows = 7,
    .cols = 7,
    .nzs = 9,
    .ellpacks = ellpacks
} ;

char buf[4096] ;

int main(void) {
    FILE *file = fopen("./resources/symm_matrix.mm", "r") ;
    SCPA_MMLOADER_HLL_LOADER_DATA loader ;
    if (file == NULL) {
        perror("Error opening file: ") ;
        return -1 ;
    }

    if (SCPA_HLL_DIRECT_LOADER_Init(file, &loader, 7)) {
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

    for (int i = 0; i < 1 ; i++) {
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