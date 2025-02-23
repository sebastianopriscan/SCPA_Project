#include <stdio.h>
#include <math.h>

#include "matrix_market_loaders/mm_loader.h"


int correctColumnMat1[4] = {0,3,1,0} ;
double correctNzMat1[4] = {1.0, 6.0, 10.5, 0} ;

int correctColumnMat2[6] = {2,0,0,1,3,4} ;
double correctNzMat2[6] = {0.015, 0., 0., 250.5, -280, 33.32} ;

int correctColumnMat3[1] = {4} ;
double correctNzMat3[1] = {12} ;

ELL_LOADER_DATA ellpacks[3] = {
    {
        .rows = 2,
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
    {
        .rows = 1,
        .cols = 5,
        .maxnz = 1,
        .columnMat = correctColumnMat3,
        .nzMat = correctNzMat3,
    },
} ;

HLL_LOADER_DATA correct = {
    .hack_size = 2,
    .rows = 5,
    .cols = 5,
    .nzs = 8,
    .ellpacks = ellpacks
} ;

char buf[4096] ;

int main(void) {
    FILE *file = fopen("./resources/matrix.mm", "r") ;
    SCPA_MMLOADER_HLL_LOADER_DATA loader ;
    if (file == NULL) {
        perror("Error opening file: ") ;
        return -1 ;
    }

    if (SCPA_HLL_DIRECT_LOADER_Init(file, &loader, 2)) {
        return -1 ;
    }

    HLL_LOADER_DATA *cast = loader.data ;

    if (cast->rows != correct.rows) return -1 ;
    if (cast->cols != correct.cols) return -1 ;
    if (cast->nzs != correct.nzs) return -1 ;
    if (cast->hack_size != correct.hack_size) return -1 ;

    for (int i = 0; i < 3 ; i++) {
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