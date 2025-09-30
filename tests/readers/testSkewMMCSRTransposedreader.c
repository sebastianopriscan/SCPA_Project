#include <stdio.h>
#include <math.h>

#include "matrix_market_loaders/mm_loader.h"


int correctRowIndexes[7] = {0, 1, 2, 2, 3, 4, 5} ;
int correctColIndexes[6] = {4,5,6,0,1,3} ;
double correctnzs[6] = {-0.433, -2.16, 8.43, 0.433, 2.16, -8.43} ;

int nzNum = 6 ;

CSR_LOADER_DATA correct = {
    .rowIdxs = correctRowIndexes,
    .colIdxs = correctColIndexes,
    .nzs = correctnzs,
    .rows = 7, .cols = 7
} ;

char buf[4096] ;

int main(void) {
    FILE *file = fopen("./resources/skew_matrix.mm", "r") ;
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

terminate:

    fclose(file) ;
    SCPA_CSR_TRANSPOSED_LOADER_Destroy(&loader) ;

    return retval ;
}