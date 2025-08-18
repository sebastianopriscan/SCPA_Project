#include "parallel/omp/csr/scpa_csr_omp_kernel.h"
#include <omp.h>


/**
 * Considerations on the kernel:
 * This kernel represents a straight parallelization of the serial code,
 * might be limited by the fact that such code creates a nested loop to bind
 * the correct row index to the column indexes/values. This loop is non-rectangular,
 * so collapsing the first loop is not possible.
 * Another possible way of tackling the problem might be parallelizing the second loop
 */

void SCPA_CSR_OMP_KERNEL(IN SCPA_MMLOADER_CSR_LOADER_DATA *matrix, IN double* x, OUT double *y) {

    CSR_LOADER_DATA *data = matrix->data ;

    double last_shared_error = 0. ;

    #pragma omp parallel shared(last_shared_error)
    {
        #pragma omp for schedule(guided, 4) nowait
        for (int i = 0 ; i < data->rows -1 ; i++) {
            double e = 0. ;
            double t, k ;
            int start = data->rowIdxs[i] ;
            int end = data->rowIdxs[i+1] ;
            for (int j = start ; j < end ; j++) {
                int colIdx = data->colIdxs[j] ;
                t = y[i] ;
                k = data->nzs[j] * x[colIdx] + e ;
                y[i] = t + k ;
                e = (t-y[i]) + k ;
            }
        }

        double last_priv_e = 0. ;
        double last_priv_s = 0. ;
        double last_priv_t = 0. ;
        double last_priv_k = 0. ;

        #pragma omp for nowait
        for (int j = data->rowIdxs[data->rows-1] ; j < data->nzNum ; j++) {
            int colIdx = data->colIdxs[j] ;
            last_priv_t = last_priv_s ;
            last_priv_k = data->nzs[j] * x[colIdx] + last_priv_e ;
            last_priv_s = last_priv_t + last_priv_k ;
            last_priv_e = (last_priv_t-last_priv_s) + last_priv_k ;
        }

        #pragma omp critical
        {
            last_priv_t = y[data->rows -1] ;
            last_priv_k = last_priv_s + last_shared_error ;
            y[data->rows -1] = last_priv_t + last_priv_k ;
            last_shared_error = (last_priv_t - y[data->rows -1]) + last_priv_k ;
        }
    }

    return ;
}