#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "matrix_market_loaders/mm_loader.h"
#include "matrix_market_loaders/mm_iterators.h"

#define as_impl_data(data) (CSR_LOADER_DATA *)(data)


//TODO : CHECK
static inline void reshape_array(CSR_LOADER_DATA *data, int row, int col, double nz) {
    int countAll = 0 ;
    int count = 0 ;
    int countMinus = 0 ;
    for (int i = 0; i < data->rows; i++) {
        countAll += data->rowIdxs[i] ;
        if (i < row)
            countMinus += data->rowIdxs[i] ;
        if (i <= row)
            count += data->rowIdxs[i] ;
    }

    int temp1, temp2 = data->colIdxs[countMinus], mod = 1;
    double nz1, nz2 = data->nzs[countMinus] ;
    int insertPoint = countMinus ;
    for (int i = countMinus ; i < countAll ; i++) {
        if (data->colIdxs[i] > col || i >= count) {
            if (mod) {
                temp1 = data->colIdxs[i+1] ;
                data->colIdxs[i+1] = temp2 ;
                nz1 = data->nzs[i+1] ;
                data->nzs[i+1] = nz2 ;
            } else {
                temp2 = data->colIdxs[i+1] ;
                data->colIdxs[i+1] = temp1 ;
                nz2 = data->nzs[i+1] ;
                data->nzs[i+1] = nz1 ;
            }

            mod = (mod +1) %2 ;
        } else {
            insertPoint++ ;
            temp2 = data->colIdxs[i+1] ;
            nz2 = data->nzs[i+1] ;
        }
    }

    data->colIdxs[insertPoint] = col ;
    data->nzs[insertPoint] = nz ;
    data->rowIdxs[row]++ ;

}

static double SCPA_MMLOADER_ReadAt(SCPA_MMLOADER_CSR_LOADER_DATA *loader, int row, int col) {
    CSR_LOADER_DATA *data = (CSR_LOADER_DATA *) loader->data ;

    for (int i = data->rowIdxs[row] ; i < data->rowIdxs[row+1] ; i++) 
        if (data->colIdxs[i] == col) return data->nzs[i] ;

    return 0. ;
}

int SCPA_CSR_DIRECT_LOADER_Init(FILE *file, SCPA_MMLOADER_CSR_LOADER_DATA *out) {

    SCPA_MM_ITERATOR *iterator = SCPA_MM_ITERATOR_Create(file) ;
    if(iterator == NULL) {
        return -1 ;
    }

    out->data = malloc(sizeof(CSR_LOADER_DATA) + sizeof(int)*(iterator->rows + iterator->nz) 
        + sizeof(double)*(iterator->nz) ) ;

    if (out->data == NULL) {
        return errno ;
    }
    CSR_LOADER_DATA *data = out->data ;
    memset(data, 0, sizeof(CSR_LOADER_DATA) + sizeof(int)*(iterator->rows + iterator->nz) 
        + sizeof(double)*(iterator->nz)) ;
    //memset(data + sizeof(CSR_LOADER_DATA), -1, sizeof(double)*(iterator->rows)) ;
    data->rowIdxs = (int *)((void *)data + sizeof(CSR_LOADER_DATA)) ;
    data->colIdxs = (int *)((void *)data->rowIdxs + sizeof(int)*iterator->rows) ;
    data->nzs = (double *)((void *)data->colIdxs + sizeof(int)*iterator->nz) ;
    data->rows = iterator->rows ;
    data->cols = iterator->cols ;

    SCPA_MM_ENTRY *entry = iterator->next(iterator) ;
    while (entry->coordx != -1)
    {
        int x = entry->coordx -1 ;
        int y = entry->coordy -1 ; 

        reshape_array(data, x, y, entry->value) ;

        free(entry) ;
        entry = iterator->next(iterator) ;
    }
    free(entry) ;
    free(iterator) ;

    int cumulated = data->rowIdxs[0] ;
    data->rowIdxs[0] = 0 ;

    for (int i = 1 ; i < data->rows ; i++) {
        int temp = data->rowIdxs[i] ;
        data->rowIdxs[i] = cumulated ;
        cumulated += temp ;
    }
    
    out->SCPA_MMLOADER_ReadAt = SCPA_MMLOADER_ReadAt ;
    
    return 0 ;
}


int SCPA_CSR_DIRECT_LOADER_Destroy(SCPA_MMLOADER_CSR_LOADER_DATA *loader) {
    free(loader->data) ;
}
