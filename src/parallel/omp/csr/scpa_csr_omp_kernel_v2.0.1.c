#include "parallel/omp/csr/scpa_csr_omp_kernel.h"
#include <stdlib.h>
#include <omp.h>

static inline int binary_search_index(CSR_LOADER_DATA* data, int startIdx) {

    if (startIdx >= data->rowIdxs[data->rows -1]) return data->rows -1 ;

    int end = data->rows ;
    int start = 0 ;
    int pivot = (end-start) / 2 ;

    while((data->rowIdxs[pivot] > startIdx) || (startIdx >= data->rowIdxs[pivot +1])) {
        if (startIdx >= data->rowIdxs[pivot]) {
            start = pivot ;
            pivot = start + (end-start) / 2 ;
        }
        else {
            end = pivot ;     
            pivot = start + (end -start) / 2 ;
        }
    }

    return pivot ;
}

/**
 * Considerations on the kernel:
 * This kernel tries to overcome the limitation of the v0 one
 * by better dividing the data between the threads: such thing
 * is accomplished by not dividing the rows among the thread,
 * which might lead to scenarios where a thread is assigned
 * a subset of the rows containing a small part of the non-zeroes
 * while another thread gets a subset which contains more, so
 * in this case the non-zeroes are divided
 * This version tries to reduce the internal 
 */

void SCPA_CSR_OMP_KERNEL(IN SCPA_MMLOADER_CSR_LOADER_DATA *matrix, IN double* x, OUT double *y) {

    CSR_LOADER_DATA *data = matrix->data ;

    double *last_shared_error = malloc(sizeof(double) * data->rows) ;
    if (last_shared_error == NULL) {
        fprintf(stderr, "Error allocating memory. Returning...\n") ;
        return ;
    }

    #pragma omp parallel shared(last_shared_error)
    {
        #pragma omp for
        for (int i = 0 ; i < data->rows; i++) {
            last_shared_error[i] = 0. ;
        }

        int tid = omp_get_thread_num() ;
        int threads = omp_get_num_threads() ;

        int nzLoad = data->nzNum / threads ;
        int modulus = tid < data->nzNum % threads ? tid : data->nzNum % threads ;
        int extra = tid < data->nzNum % threads ? 1 : 0 ;

        int startIdx = nzLoad * tid + modulus ;
        int count = nzLoad + extra ;

        double e = 0. ;
        double t, k ;
        double s = 0. ;

        int rowIdx = binary_search_index(data, startIdx) ;
        int limitRowIdx = rowIdx == data->rows -1 ? data->nzNum : data->rowIdxs[rowIdx+1] ;
        int limit = limitRowIdx < (startIdx + count) ? limitRowIdx : (startIdx + count) ;
        int start = startIdx ;

        while (1) {
            for(int i = start; i < limit; i++) {
                int colIdx = data->colIdxs[i] ;
                t = s ;
                k = data->nzs[i] * x[colIdx] + e ;
                s = t + k ;
                e = (t-s) + k ;
            }

            #pragma omp critical
            {
                t = y[rowIdx] ;
                k = s + last_shared_error[rowIdx] ;
                y[rowIdx] = t + k ;
                last_shared_error[rowIdx] = (t-s) + k ;
                e = 0. ;
                s = 0. ;
            }

            if (limit == startIdx + count) break;
            
            start = limit ;
            rowIdx++;
            limitRowIdx = rowIdx == data->rows -1 ? data->nzNum : data->rowIdxs[rowIdx+1] ;
            limit = limitRowIdx < (startIdx + count) ? limitRowIdx : (startIdx + count) ;
        }        

    }

    return ;
}