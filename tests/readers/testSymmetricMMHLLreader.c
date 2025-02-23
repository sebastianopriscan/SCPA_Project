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
        return -1 ;
    }

    HLL_LOADER_DATA *cast = loader.data ;

    if (cast->rows != correct.rows) return -1 ;
    if (cast->cols != correct.cols) return -1 ;
    if (cast->nzs != correct.nzs) return -1 ;
    if (cast->hack_size != correct.hack_size) return -1 ;

    for (int i = 0; i < 1 ; i++) {
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