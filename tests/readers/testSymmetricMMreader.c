#include <stdio.h>
#include <math.h>

#include "matrix_market_loaders/mm_loader.h"

#define as_impl_data(data) (CSR_LOADER_DATA *)(data)


struct _CSR_LOADER_DATA {
    int *rowIdxs ;
    int *colIdxs ;
    double *nzs ;
    int rows, cols ;
} ;
typedef struct _CSR_LOADER_DATA CSR_LOADER_DATA ;

int correctRowIndexes[7] = {0, 2, 2, 4, 5, 5, 7} ;
int correctColIndexes[9] = {2,6,0,2,5,3,5,0,6} ;
double correctnzs[9] = {10.5, 3.0, 10.5, -0.145, 7.9, 7.9, 2.0, 3.0, 0.433} ;

int nzNum = 9 ;

CSR_LOADER_DATA correct = {
    .rowIdxs = correctRowIndexes,
    .colIdxs = correctColIndexes,
    .nzs = correctnzs,
    .rows = 7, .cols = 7
} ;

char buf[4096] ;

int main(void) {
    FILE *file = fopen("./resources/symm_matrix.mm", "r") ;
    SCPA_MMLOADER loader ;
    if (file == NULL) {
        perror("Error opening file: ") ;
        return -1 ;
    }

    if (SCPA_CSR_DIRECT_LOADER_Init(file, &loader)) {
        return -1 ;
    }

    CSR_LOADER_DATA *cast = as_impl_data(loader.data) ;

    for (int i = 0 ; i < correct.rows ; i++)
        if (cast->rowIdxs[i] != correct.rowIdxs[i]) return -1 ;

    for (int i = 0; i < nzNum ; i++) {
        if (cast->colIdxs[i] != correct.colIdxs[i]) return -1 ;
        if (fabs(correct.nzs[i] - cast->nzs[i]) > 0.01) return -1 ;
    }

    return 0 ;
}