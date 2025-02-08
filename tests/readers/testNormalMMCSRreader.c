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

int correctRowIndexes[5] = {0,2,3,4,7} ;
int correctColIndexes[8] = {0,3,1,2,1,3,4,4} ;
double correctnzs[8] = {1.0, 6.0, 10.5, 0.015, 250.5, -280.0, 33.32, 12.0} ;

CSR_LOADER_DATA correct = {
    .rowIdxs = correctRowIndexes,
    .colIdxs = correctColIndexes,
    .nzs = correctnzs,
    .rows = 5, .cols = 5
} ;

char buf[4096] ;

int main(void) {
    FILE *file = fopen("./resources/matrix.mm", "r") ;
    SCPA_MMLOADER loader ;
    if (file == NULL) {
        perror("Error opening file: ") ;
        return -1 ;
    }

    if (SCPA_CSR_DIRECT_LOADER_Init(file, &loader)) {
        return -1 ;
    }

    CSR_LOADER_DATA *cast = as_impl_data(loader.data) ;

    for (int i = 0 ; i < 5 ; i++)
        if (cast->rowIdxs[i] != correct.rowIdxs[i]) return -1 ;

    for (int i = 0; i < 8 ; i++) {
        if (cast->colIdxs[i] != correct.colIdxs[i]) return -1 ;
        if (fabs(correct.nzs[i] - cast->nzs[i]) > 0.01) return -1 ;
    }

    return 0 ;
}