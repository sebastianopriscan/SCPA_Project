#include "parallel/omp/csr/scpa_csr_omp_kernel.h"
#include <omp.h>


/**
 * Considerations on the kernel:
 * With respect to v0, this kernel tries shifting the parallelization to the inner loop
 * Too much trashing and really ill-conditioned
 */

void SCPA_CSR_OMP_KERNEL(IN SCPA_MMLOADER_CSR_LOADER_DATA *matrix, IN double* x, OUT double *y) {

    CSR_LOADER_DATA *data = matrix->data ;

    double last_shared_error = 0. ;

    for (int i = 0 ; i < data->rows -1 ; i++) {
        int start = data->rowIdxs[i] ;
        int end = data->rowIdxs[i+1] ;
        #pragma omp parallel shared(last_shared_error)
        {
            double e = 0. ;
            double s = 0. ;
            double t, k ;
            #pragma omp for schedule(static) nowait
            for (int j = start ; j < end ; j++) {
                int colIdx = data->colIdxs[j] ;
                t = s ;
                k = data->nzs[j] * x[colIdx] + e ;
                s = t + k ;
                e = (t-s) + k ;
            }

            #pragma omp critical 
            {
                t = y[i] ;
                k = s + last_shared_error + e ;
                y[i] = t + k ;
                last_shared_error = (t - y[i]) + k ;
            }

            #pragma omp single
            {
                last_shared_error = 0. ;
            }
        }
    }

    #pragma omp parallel shared(last_shared_error)
    {
        double last_priv_e = 0. ;
        double last_priv_s = 0. ;
        double last_priv_t = 0. ;
        double last_priv_k = 0. ;

        #pragma omp for schedule(static) nowait
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
            last_priv_k = last_priv_s + last_shared_error + last_priv_e ;
            y[data->rows -1] = last_priv_t + last_priv_k ;
            last_shared_error = (last_priv_t - y[data->rows -1]) + last_priv_k ;
        }
    }

    return ;
}