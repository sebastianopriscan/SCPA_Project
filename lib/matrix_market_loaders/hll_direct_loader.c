#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "matrix_market_loaders/mm_loader.h"
#include "matrix_market_loaders/mm_iterators.h"

#define as_impl_data(data) (HLL_LOADER_DATA *)(data)

struct _ELL_LOADER_DATA {
    int rows ;
    int cols ;
    int maxnz ;
    int *columnMat ;
    double *nzMat ;
} ;
typedef struct _ELL_LOADER_DATA ELL_LOADER_DATA ;

#define ELL_LOADER_DATA_SIZE(rows, nz) (sizeof(ELL_LOADER_DATA) + sizeof(int) * (rows) * (nz) + sizeof(double) * (rows) * (nz))

struct _HLL_LOADER_DATA {
    int hack_size ;
    int rows ;
    int cols ;
    int nzs ;
    ELL_LOADER_DATA *ellpacks ;
} ;
typedef struct _HLL_LOADER_DATA HLL_LOADER_DATA ;

struct _maxnzs {
    int size ;
    int *values ;
} ;
typedef struct _maxnzs maxnzs ;

static int getMaxNzs(IN SCPA_MM_ITERATOR *iterator, IN int hackSize, MALLOCD OUT maxnzs **nz) {

    if (hackSize <= 0) return -1 ;

    int *counters = malloc(sizeof(int) * iterator->rows) ;
    if (counters == NULL) {
        return -1 ;
    }
    memset(counters, 0, sizeof(int) * iterator->rows) ;

    SCPA_MM_ITERATOR_Reset(iterator) ;

    SCPA_MM_ENTRY *entry = iterator->next(iterator) ;
    while(entry->coordx != -1) {

        counters[entry->coordx -1]++ ;

        free(entry) ;
        entry = iterator->next(iterator) ;
    }

    SCPA_MM_ITERATOR_Reset(iterator) ;

    int entries = iterator->rows % hackSize ? (iterator->rows + hackSize) / hackSize : iterator->rows / hackSize ;
    maxnzs *retVal = malloc(sizeof(maxnzs) + sizeof(int) * entries) ;
    if (retVal == NULL) {
        free(counters) ;
        return 0 ;
    }
    retVal->size = entries ;
    retVal->values = (int *)((char *)retVal + sizeof(maxnzs)) ;

    int index = 0;
    int maxValue = -1 ;
    for (int i = 0; i < iterator->rows; i++) {

        if (i % hackSize == 0 && i != 0) {
            retVal->values[index] = maxValue ;
            maxValue = -1 ;
            index++;
        }

        if (maxValue < counters[i]) maxValue = counters[i] ;
    }
    retVal->values[index] = maxValue ;

    *nz = retVal ;
    free(counters) ;
    return 1 ;
}

static int reshape_ell_data(HLL_LOADER_DATA *data, int row, int col, double value) {

    const int ell_row = row % data->hack_size ;
    const ELL_LOADER_DATA *ell_entry = data->ellpacks + (row / data->hack_size) ;

    int *ell_loader_data_row = ell_entry->columnMat + (ell_entry->maxnz * ell_row) ;    
    double *ell_loader_data_nz = ell_entry->nzMat + (ell_entry->maxnz * ell_row) ;

    int colIdx1, colIdx2 = col ;
    double nz1, nz2 = value ;
    int switching = 0, mod = 1 ;

    for (int i = 0; i < ell_entry->maxnz; i++) {
        if (switching) {
            if (mod) {
                colIdx1 = ell_loader_data_row[i] ;
                nz1 = ell_loader_data_nz[i] ;
                ell_loader_data_row[i] = colIdx2 ;
                ell_loader_data_nz[i] = nz2 ;
                if (colIdx1 == 0) break ; //TODO : CHECK COMPLIANCE WITH ELLPACK FORMAT SPECIFICATION
            } else {
                colIdx2 = ell_loader_data_row[i] ;
                nz2 = ell_loader_data_nz[i] ;
                ell_loader_data_row[i] = colIdx1 ;
                ell_loader_data_nz[i] = nz1 ;
                if (colIdx2 == 0) break ; //TODO : CHECK COMPLIANCE WITH ELLPACK FORMAT SPECIFICATION
            }

            mod = mod % 2 ;
        } else {
            if (ell_loader_data_row[i] == 0) {
                ell_loader_data_row[i] = col ;
                ell_loader_data_nz[i] = value ;
                break ; //TODO : CHECK COMPLIANCE WITH ELLPACK FORMAT SPECIFICATION
            }
            if (ell_loader_data_row[i] < col) {
                continue;
            }
            switching = 1 ;
            colIdx2 = ell_loader_data_row[i] ;
            nz2 = ell_loader_data_nz[i] ;
            ell_loader_data_row[i] = col ;
            ell_loader_data_nz[i] = value ;
        }
    }
}

static void reindex_ell_columns(HLL_LOADER_DATA *data) {
    
    int size = data->hack_size == 1 ? data->rows : (data->rows + data->hack_size) / data->hack_size ;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < data->ellpacks[i].maxnz * data->ellpacks[i].rows; j++) {
            if (data->ellpacks[i].columnMat[j]) {
                data->ellpacks[i].columnMat[j]-- ;
            }
        }
    }
}

int SCPA_HLL_DIRECT_LOADER_Init(FILE *file, SCPA_MMLOADER *out, int hackSize) {

    SCPA_MM_ITERATOR *iterator = SCPA_MM_ITERATOR_Create(file) ;
    if(iterator == NULL) {
        return -1 ;
    }

    maxnzs *maxima ;
    int result = getMaxNzs(iterator, hackSize, &maxima) ;

    int instances = iterator->rows % hackSize ? (iterator->rows + hackSize) / hackSize : iterator->rows / hackSize ;
    int last_row_size = iterator->rows % hackSize ? iterator->rows % hackSize : hackSize ;

    int ell_size = sizeof(HLL_LOADER_DATA) ;
    for(int i = 0; i < maxima->size -1 ; i++) {
        ell_size += ELL_LOADER_DATA_SIZE(hackSize, maxima->values[i]) ;
    }
    ell_size += ELL_LOADER_DATA_SIZE(last_row_size, maxima->values[maxima->size -1]) ;

    out->data = malloc(ell_size) ;

    if (out->data == NULL) {
        return errno ;
    }
    HLL_LOADER_DATA *data = (HLL_LOADER_DATA *) out->data ;
    memset(data, 0, ell_size) ;

    data->rows = iterator->rows ;
    data->cols = iterator->cols ;
    data->nzs = iterator->nz ;
    data->hack_size = hackSize ;
    data->ellpacks = (ELL_LOADER_DATA *)((char *)data + sizeof(HLL_LOADER_DATA)) ;

    int ell_cumulated_size = sizeof(HLL_LOADER_DATA) + sizeof(ELL_LOADER_DATA) * instances ;


    for (int i = 0; i < maxima->size -1; i++) {
        data->ellpacks[i].rows = hackSize ;
        data->ellpacks[i].cols = iterator->cols ;
        data->ellpacks[i].maxnz = maxima->values[i] ;
        data->ellpacks[i].columnMat = (int *)((char *)data + ell_cumulated_size) ;
        data->ellpacks[i].nzMat = (double *)((char *)data + ell_cumulated_size + sizeof(int) * hackSize * maxima->values[i]) ;
        ell_cumulated_size += sizeof(int) * hackSize * maxima->values[i] + sizeof(double) * hackSize * maxima->values[i] ;
    }
    data->ellpacks[maxima->size -1].rows = last_row_size ;
    data->ellpacks[maxima->size -1].cols = iterator->cols ;
    data->ellpacks[maxima->size -1].maxnz = maxima->values[maxima->size -1] ;
    data->ellpacks[maxima->size -1].columnMat = (int *)((char *)data + ell_cumulated_size) ;
    data->ellpacks[maxima->size -1].nzMat = (double *)((char *)data + ell_cumulated_size 
            + sizeof(int) * (last_row_size) * maxima->values[maxima->size -1]) ;

    SCPA_MM_ENTRY *entry = iterator->next(iterator) ;
    while (entry->coordx != -1)
    {
        int x = entry->coordx -1 ;
        int y = entry->coordy ; 

        reshape_ell_data(data, x, y, entry->value) ;

        free(entry) ;
        entry = iterator->next(iterator) ;
    }
    free(entry) ;
    free(iterator) ;

    reindex_ell_columns(data) ;

    return 0 ;
}

int SCPA_HLL_DIRECT_LOADER_Destroy(SCPA_MMLOADER *loader) {

}