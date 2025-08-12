#include "matrix_market_loaders/mm_loader.h"
#include "vector_loaders/vector_loaders.h"
#include "serial/csr/scpa_csr_serial_kernel.h"
#include <stdlib.h>
#include <string.h>

int main(void) {

    FILE* file = fopen("resources/pattern_matrix.mm", "r") ;
    if (file == NULL) {
        perror("Error opening file: ") ;
        return 1 ;
    }

    SCPA_MMLOADER_CSR_LOADER_DATA out ;
    if (SCPA_CSR_DIRECT_LOADER_Init(file, &out)) {
        fprintf(stderr, "Error loading CSR Matrix. Exiting...\n") ;
        fclose(file) ;
        return -1;
    }

    double *vector = SCPA_VECTOR_LoadFromFile("resources/test_vector.vec") ;
    if (vector == NULL) {
        fprintf(stderr, "Error loading vector. Exiting...\n") ;
        SCPA_CSR_DIRECT_LOADER_Destroy(&out) ;
        fclose(file) ;
        return -1 ;
    }

    double *result = malloc(out.data->rows * sizeof(double)) ;
    if (result == NULL) {
        perror("Error allocating output buffer: ") ;
        free(vector) ;
        SCPA_CSR_DIRECT_LOADER_Destroy(&out) ;
        fclose(file) ;
        return -1 ;
    }
    memset(result, 0, out.data->rows * sizeof(double)) ;

    SCPA_CSR_SERIAL_KERNEL(&out, vector, result) ;

    int retVal = 0;
    if (abs(result[0] - 5.0) > 0.01) {
        retVal = -1 ;
        goto terminate ;
    }
    if (abs(result[1] - 2.0) > 0.01) {
        retVal = -1 ;
        goto terminate ;
    }
    if (abs(result[2] - 3.0) > 0.01) {
        retVal = -1 ;
        goto terminate ;
    }
    if (abs(result[3] - 11.0) > 0.01) {
        retVal = -1 ;
        goto terminate ;
    }
    if (abs(result[4] - 5.0) > 0.01) {
        retVal = -1 ;
        goto terminate ;
    }


terminate:

    free(result) ;
    free(vector) ;
    SCPA_CSR_DIRECT_LOADER_Destroy(&out) ;
    fclose(file) ;

    return retVal ;
}