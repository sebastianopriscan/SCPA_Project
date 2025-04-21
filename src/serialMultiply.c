#include <stdlib.h>
#include <time.h>

#include "args/parse_args.h"
#include "serial/csr/scpa_csr_serial_kernel.h"


int main(int argc, char **argv) {
    SCPA_ParsedArgs *args = SCPA_ParseArgs(argc, argv) ;

    if (args == NULL) return 1 ;

    if (args->loaderClass != SCPA_DIRECT_CSR_CLASS) {
        fprintf(stderr, "Serial kernel supports only simple CSR\n") ;
        SCPA_HLL_DIRECT_LOADER_Destroy(args->loader) ;
        free(args->loader) ;
        free(args->output) ;
        free(args) ;
        return 1;
    }

    double *result = malloc(sizeof(double) * ((SCPA_MMLOADER_CSR_LOADER_DATA *)args->loader)->data->cols) ;
    if (result == NULL) {
        fprintf(stderr, "Serial kernel supports only simple CSR\n") ;
        SCPA_HLL_DIRECT_LOADER_Destroy(args->loader) ;
        free(args->loader) ;
        free(args->output) ;
        free(args) ;
        return 1;
    }

    struct timespec start,end ;

    clock_gettime(CLOCK_BOOTTIME, &start) ;
    SCPA_CSR_SERIAL_KERNEL(
        (SCPA_MMLOADER_CSR_LOADER_DATA *)args->loader,
        args->output,
        result
    ) ;
    clock_gettime(CLOCK_BOOTTIME, &end) ;


    free(result) ;
    SCPA_CSR_DIRECT_LOADER_Destroy(args->loader) ;
    free(args->loader) ;
    free(args->output) ;
    free(args) ;
    printf("%ld\n", (end.tv_sec - start.tv_sec)*NANOSEC_PER_SEC + end.tv_nsec - start.tv_nsec) ;
}