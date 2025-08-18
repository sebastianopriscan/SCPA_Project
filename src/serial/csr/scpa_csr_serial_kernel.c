#include "serial/csr/scpa_csr_serial_kernel.h"

void SCPA_CSR_SERIAL_KERNEL(IN SCPA_MMLOADER_CSR_LOADER_DATA *matrix, IN double* x, OUT double *y) {

    CSR_LOADER_DATA *data = matrix->data ;

    int cumulated = 0, cumulated_end = 0 ;
    for (int i = 0 ; i < data->rows -1 ; i++) {
        double e = 0. ;
        double t, k ;
        cumulated_end = data->rowIdxs[i+1] ;
        for (int j = cumulated ; j < cumulated_end ; j++) {
            int colIdx = data->colIdxs[j] ;
            t = y[i] ;
            k = data->nzs[j] * x[colIdx] + e ;
            y[i] = t + k ;
            e = (t-y[i]) + k ;
        }
        cumulated = cumulated_end ;
    }

    double e = 0. ;
    double t, k ;
    for (int j = cumulated ; j < data->nzNum ; j++) {
        int colIdx = data->colIdxs[j] ;
        t = y[data->rows -1] ;
        k = data->nzs[j] * x[colIdx] + e ;
        y[data->rows -1] = t + k ;
        e = (t-y[data->rows -1]) + k ;
    }

    return ;
}