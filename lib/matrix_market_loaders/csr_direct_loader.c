#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "matrix_market_loaders/mm_loader.h"
#include "matrix_market_loaders/mm_iterators.h"

#define as_impl_data(data) (CSR_LOADER_DATA *)(data)

static struct _CSR_LOADER_DATA {
    int *rowIdxs ;
    int *colIdxs ;
    int *nzs ;
    int rows, cols ;
} ;
typedef struct _CSR_LOADER_DATA CSR_LOADER_DATA ;

//TODO : CHECK
static inline void reshape_array(CSR_LOADER_DATA *data, int row, int col, double nz) {
    int countAll = 0 ;
    int count = 0 ;
    int countMinus = 0 ;
    for (int i = 0; i < data->rows; i++) {
        countAll += data->rowIdxs[i] ;
        if (data->rowIdxs[i] < row)
            countMinus += data->rowIdxs[i] ;
        if (data->rowIdxs[i] <= row)
            count += data->rowIdxs[i] ;
    }

    int temp1, temp2 = data->colIdxs[count], mod = 0;
    int insertPoint = countMinus ;
    for (int i = countMinus ; i < countAll ; i++) {
        if (data->colIdxs[i] > col || i >= count) {
            if (mod) {
                temp1 = data->colIdxs[i+1] ;
                data->colIdxs[i+1] = temp2 ;
            } else {
                temp2 = data->colIdxs[i+1] ;
                data->colIdxs[i+1] = temp1 ;
            }

            mod = (mod +1) %2 ;
        } else {
            insertPoint++ ;
        }
    }

    data->colIdxs[insertPoint] = col ;
    data->nzs[insertPoint] = nz ;

}

int SCPA_CSR_DIRECT_LOADER_Init(FILE *file, SCPA_MMLOADER *out) {

    SCPA_MM_ITERATOR *iterator = SCPA_MM_ITERATOR_Create(file) ;
    if(iterator == NULL) {
        return -1 ;
    }

    out->data = malloc(sizeof(CSR_LOADER_DATA) + sizeof(double)*(iterator->rows + 2*iterator->nz) ) ;
    if (out->data == NULL) {
        return errno ;
    }
    CSR_LOADER_DATA *data = out->data ;
    data->rowIdxs = data + sizeof(CSR_LOADER_DATA) ;
    data->colIdxs = data->rowIdxs + sizeof(double)*iterator->rows ;
    data->nzs = data->colIdxs + sizeof(double)*iterator->nz ;

    memset(data, 0, sizeof(CSR_LOADER_DATA) + sizeof(double)*(iterator->rows + 2*iterator->nz)) ;
    //memset(data + sizeof(CSR_LOADER_DATA), -1, sizeof(double)*(iterator->rows)) ;

    SCPA_MM_ENTRY *entry = iterator->next() ;
    while (entry->coordx != -1)
    {
        int x = entry->coordx -1 ;
        int y = entry->coordy -1 ; 

        reshape_array(data, x, y, entry->value) ;

        entry = iterator->next() ;
    }
    
    out->SCPA_MMLOADER_ReadAt = SCPA_MMLOADER_ReadAt ;
}


int SCPA_CSR_DIRECT_LOADER_Destroy(SCPA_MMLOADER *loader) {
    free(loader->data) ;
}

static int SCPA_MMLOADER_ReadAt(SCPA_MMLOADER *loader, int row, int col) {
    CSR_LOADER_DATA *data = (CSR_LOADER_DATA *) loader->data ;

    for (int i = data->rowIdxs[row] ; i < data->rowIdxs[row+1] ; i++) 
        if (data->colIdxs[i] == col) return data->nzs[i] ;

    return 0 ;
}