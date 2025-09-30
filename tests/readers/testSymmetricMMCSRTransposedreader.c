#include <stdio.h>
#include <math.h>

#include "matrix_market_loaders/mm_loader.h"


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
    SCPA_MMLOADER_CSR_LOADER_DATA loader ;
    if (file == NULL) {
        perror("Error opening file: ") ;
        return -1 ;
    }

    if (SCPA_CSR_TRANSPOSED_LOADER_Init(file, &loader)) {
        fclose(file) ;
        return -1 ;
    }

    CSR_LOADER_DATA *cast = loader.data ;

    int retval = 0 ;

    for (int i = 0 ; i < correct.rows ; i++)
        if (cast->rowIdxs[i] != correct.rowIdxs[i]) {
            retval = -1 ;
            goto terminate ;
        }

    for (int i = 0; i < nzNum ; i++) {
        if (cast->colIdxs[i] != correct.colIdxs[i]) {
            retval = -1 ;
            goto terminate ;
        }
        if (fabs(correct.nzs[i] - cast->nzs[i]) > 0.01) {
            retval = -1 ;
            goto terminate ;
        }
    }

terminate :

    fclose(file) ;
    SCPA_CSR_TRANSPOSED_LOADER_Destroy(&loader) ;

    return retval ;
}