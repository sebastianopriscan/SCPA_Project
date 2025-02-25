#ifndef SCPA_CSR_SERIAL_KERNEL_H
#define SCPA_CSR_SERIAL_KERNEL_H

#include "common/definitions.h"
#include "matrix_market_loaders/mm_loader.h"

/**
 * Baseline CSR serial kernel
 * @param matrix : Pointer to an SCPA_MM_LOADER_CSR_LOADER_DATA loader, that contains, in its data
 *                 field, the matrix in CSR format with info on its size, too
 * @param vector : Pointer to a dense vector to be multiplied with matrix, the user MUST ensure the dimensions match
 * @param result : Pointer to a memory area on which the result shall be dumped, the user MUST ensure the dimensions match
 * 
 * @returns an integer different than zero in case of error, 0 otherwise
 */
int SCPA_CSR_SERIAL_KERNEL(IN SCPA_MMLOADER_CSR_LOADER_DATA *matrix,
    IN double* vector, OUT double *result) ;

#endif