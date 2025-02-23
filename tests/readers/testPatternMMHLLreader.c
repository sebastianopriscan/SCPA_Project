#include <stdio.h>
#include <math.h>

#include "matrix_market_loaders/mm_loader.h"


int correctColumnMat1[6] = {0,3,1,0,2,0} ;
double correctNzMat1[6] = {1.0, 1.0, 1., 0, 1., 0} ;

int correctColumnMat2[6] = {1,3,4,4,0,0} ;
double correctNzMat2[6] = {1.0, 1., 1., 1., 0, 0} ;

ELL_LOADER_DATA ellpacks[2] = {
    {
        .rows = 3,
        .cols = 5,
        .maxnz = 2,
        .columnMat = correctColumnMat1,
        .nzMat = correctNzMat1,
    },
    {
        .rows = 2,
        .cols = 5,
        .maxnz = 3,
        .columnMat = correctColumnMat2,
        .nzMat = correctNzMat2,
    },
} ;

HLL_LOADER_DATA correct = {
    .hack_size = 3,
    .rows = 5,
    .cols = 5,
    .nzs = 8,
    .ellpacks = ellpacks
} ;

char buf[4096] ;

int main(void) {
    FILE *file = fopen("./resources/pattern_matrix.mm", "r") ;
    SCPA_MMLOADER_HLL_LOADER_DATA loader ;
    if (file == NULL) {
        perror("Error opening file: ") ;
        return -1 ;
    }

    if (SCPA_HLL_DIRECT_LOADER_Init(file, &loader, 3)) {
        return -1 ;
    }

    HLL_LOADER_DATA *cast = loader.data ;

    if (cast->rows != correct.rows) return -1 ;
    if (cast->cols != correct.cols) return -1 ;
    if (cast->nzs != correct.nzs) return -1 ;
    if (cast->hack_size != correct.hack_size) return -1 ;

    for (int i = 0; i < 2 ; i++) {
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