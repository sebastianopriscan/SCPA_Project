#include "serial/csr/scpa_csr_serial_kernel.h"

void SCPA_CSR_SERIAL_KERNEL(IN SCPA_MMLOADER_CSR_LOADER_DATA *matrix, IN double* x, OUT double *y) {

    CSR_LOADER_DATA *data = matrix->data ;

    int cumulated = 0, cumulated_end = 0 ;
    for (int i = 0 ; i < data->rows -1 ; i++) {
        cumulated_end = data->rowIdxs[i+1] ;
        for (int j = cumulated ; j < cumulated_end ; j++) {
            int colIdx = data->colIdxs[j] ;
            y[i] += data->nzs[j] * x[colIdx] ;
        }
        cumulated = cumulated_end ;
    }

    for (int j = cumulated ; j < data->nzNum ; j++) {
        int colIdx = data->colIdxs[j] ;
        y[data->rowIdxs[data->rows -1]] += data->nzs[j] * x[colIdx] ;
    }

    return ;
}