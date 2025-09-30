#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "matrix_market_loaders/mm_loader.h"
#include "matrix_market_loaders/mm_iterators.h"

#define as_impl_data(data) (CSR_LOADER_DATA *)(data)


//TODO : CHECK
static inline void reshape_array(CSR_LOADER_DATA *data, int row, int col, double nz) {

    int baseIdx = data->rowIdxs[row] ;
    int size = (row == data->rows -1 ? data->nzNum : data->rowIdxs[row+1]) ;
    int temp[2], mod = 0;
    double nzArr[2] ;
    int insertPoint = baseIdx -1 ;

    for (int i = baseIdx; i < size; i++) {
        if (data->colIdxs[i] < col) {
            insertPoint++;
        }
        if (data->colIdxs[i] == -1) {
            size = i ;
            break;
        }
    }

    temp[1] = data->colIdxs[insertPoint] ;
    nzArr[1] = data->nzs[insertPoint] ;

    for (int i = insertPoint ; i < size ; i++) {
        temp[mod] = data->colIdxs[i+1] ;
        data->colIdxs[i+1] = temp[1- mod] ;
        nzArr[mod] = data->nzs[i+1] ;
        data->nzs[i+1] = nzArr[1-mod] ;

        mod = (mod +1) %2 ;
    }

    data->colIdxs[insertPoint] = col ;
    data->nzs[insertPoint] = nz ;
}

static double SCPA_MMLOADER_ReadAt(SCPA_MMLOADER_CSR_LOADER_DATA *loader, int row, int col) {
    CSR_LOADER_DATA *data = (CSR_LOADER_DATA *) loader->data ;

    for (int i = data->rowIdxs[row] ; i < data->rowIdxs[row+1] ; i++) 
        if (data->colIdxs[i] == col) return data->nzs[i] ;

    return 0. ;
}

int SCPA_CSR_TRANSPOSED_LOADER_Init(FILE *file, SCPA_MMLOADER_CSR_LOADER_DATA *out) {

    SCPA_MM_ITERATOR *iterator = SCPA_MM_ITERATOR_Create(file) ;
    if(iterator == NULL) {
        return -1 ;
    }

    out->data = malloc(sizeof(CSR_LOADER_DATA) + sizeof(int)*(iterator->cols + iterator->nz) 
        + sizeof(double)*(iterator->nz) ) ;

    if (out->data == NULL) {
        return errno ;
    }
    CSR_LOADER_DATA *data = out->data ;
    memset(data, 0, sizeof(CSR_LOADER_DATA) + sizeof(int)*(iterator->cols)) ;
    memset((void*)data + sizeof(CSR_LOADER_DATA) + sizeof(int)*iterator->cols, -1, sizeof(int)*(iterator->nz)) ;
    memset((void*)data + sizeof(CSR_LOADER_DATA) + sizeof(int)*(iterator->cols + iterator->nz), 0, sizeof(double)*(iterator->nz)) ;
    //memset(data + sizeof(CSR_LOADER_DATA), -1, sizeof(double)*(iterator->rows)) ;
    data->rowIdxs = (int *)((void *)data + sizeof(CSR_LOADER_DATA)) ;
    data->colIdxs = (int *)((void *)data->rowIdxs + sizeof(int)*iterator->cols) ;
    data->nzs = (double *)((void *)data->colIdxs + sizeof(int)*iterator->nz) ;
    data->rows = iterator->cols ;
    data->cols = iterator->rows ;
    data->nzNum = iterator->nz ;

    SCPA_MM_ENTRY *entry = iterator->next(iterator) ;
    while (entry->coordx != -1)
    {
        int y = entry->coordy -1 ;

        data->rowIdxs[y]++ ;

        free(entry) ;
        entry = iterator->next(iterator) ;
    }
    free(entry) ;

    int cumulated = 0;
    int value = 0;
    for (int i = 0; i < data->rows; i++) {
        cumulated += value ;
        value = data->rowIdxs[i] ;
        data->rowIdxs[i] = cumulated ;
    }

    SCPA_MM_ITERATOR_Reset(iterator) ;
    entry = iterator->next(iterator) ;
    while (entry->coordx != -1)
    {
        int x = entry->coordy -1 ;
        int y = entry->coordx -1 ; 

        reshape_array(data, x, y, entry->value) ;

        free(entry) ;
        entry = iterator->next(iterator) ;
    }
    free(entry) ;
    free(iterator) ;

    out->SCPA_MMLOADER_ReadAt = SCPA_MMLOADER_ReadAt ;
    
    return 0 ;
}


int SCPA_CSR_TRANSPOSED_LOADER_Destroy(SCPA_MMLOADER_CSR_LOADER_DATA *loader) {
    free(loader->data) ;
}
