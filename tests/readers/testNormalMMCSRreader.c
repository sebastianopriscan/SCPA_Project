#include <stdio.h>
#include <math.h>

#include "matrix_market_loaders/mm_loader.h"


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
    SCPA_MMLOADER_CSR_LOADER_DATA loader ;
    if (file == NULL) {
        perror("Error opening file: ") ;
        return -1 ;
    }

    if (SCPA_CSR_DIRECT_LOADER_Init(file, &loader)) {
        fclose(file) ;
        return -1 ;
    }

    CSR_LOADER_DATA *cast = loader.data ;

    int retval = 0 ;

    for (int i = 0 ; i < 5 ; i++)
        if (cast->rowIdxs[i] != correct.rowIdxs[i]) {
            retval = -1 ;
            goto terminate ;
        }

    for (int i = 0; i < 8 ; i++) {
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
    SCPA_CSR_DIRECT_LOADER_Destroy(&loader) ;

    return retval ;
}