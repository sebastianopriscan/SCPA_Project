#include "serial/csr/scpa_csr_serial_kernel.h"
#include "sparse_matrix_cache/sparse_matrix_cache.h"
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define FGETS_BUFSIZE 2048

#define error_for_matrix(message) fprintf(stderr, message"\n", group_name, matrix_name)
#define log_for_matrix(message) printf(message"\n", group_name, matrix_name)

char group_name[512] ;
char matrix_name[512] ;
char fgets_buffer[FGETS_BUFSIZE] ;

int main(int argc, char **argv) {

    if (argc != 3) {
        fprintf(stderr, "Usage: ./progName matrix_cache_dir result_store_path") ;
        return -1 ;
    }

    char *matrix_cache_dir = argv[1], *result_store_path = argv[2] ;

    FILE *matrices = fopen("resources/comparative/matrices", "r") ;
    if (matrices == NULL) {
        perror("Error opening matrices file") ;
        return -1 ;
    }

    while (fgets(fgets_buffer, FGETS_BUFSIZE, matrices)) {
        if(sscanf(fgets_buffer, "%511s %511s\n", group_name, matrix_name) == 2) {

            char *outfilename = malloc(strlen(argv[2]) + strlen(group_name) + strlen(matrix_name) +3) ;
            if (outfilename == NULL) {
                error_for_matrix("Error allocating output file name for matrix %s %s. Skipping...") ;
                continue ;
            }

            char *cursor = stpcpy(outfilename, argv[2]) ;
            *cursor++ = '/' ;
            cursor = stpcpy(cursor, group_name) ;
            *cursor++ = '_' ;
            cursor = stpcpy(cursor, matrix_name) ;
            *cursor = '\0' ; 


            struct stat ignored ;
            if (stat(outfilename, &ignored) == 0) {
                log_for_matrix("Data for matrix %s %s already generated. Skipping...") ;
                goto loop_free_pathname ;
            }

            FILE* matrix ;
            SCPA_MMLOADER_CSR_LOADER_DATA out ;

            if (SCPA_SPCACHE_OpenMatrix(matrix_name, group_name, argv[1], &matrix) == 1) {
                error_for_matrix("Error getting matrix %s %s. Skipping...") ;
                goto loop_free_pathname;
            }
            log_for_matrix("Opened matrix %s %s") ;

            if (SCPA_CSR_DIRECT_LOADER_Init(matrix, &out) != 0) {
                error_for_matrix("Error loading matrix %s %s in CSR format. Skipping...") ;
                goto loop_free_matrix ;

            }
            log_for_matrix("Loaded matrix %s %s in CSR format") ;

            double *vector = malloc(sizeof(double) * (out.data->rows + out.data->cols)) ;
            if (vector == NULL) {
                error_for_matrix("Error allocating vector to multiply matrix %s %s with. Skipping...") ;
                goto loop_destroy_loader ;
            }

            for (int i = 0; i < out.data->cols; i++) {
                vector[i] = (double) (i % 100) ;
            }
            memset(vector + out.data->cols, 0, sizeof(double) * out.data->rows) ;


            FILE *output = fopen(outfilename, "w+") ;
            if (output == NULL) {
                error_for_matrix("Error opening output file for matrix %s %s. Skipping...") ;
                goto loop_free_vector ;
            }

            log_for_matrix("Multiplying matrix %s %s") ;
            SCPA_CSR_SERIAL_KERNEL(&out, vector, vector + out.data->cols) ;
            log_for_matrix("Multiplied matrix %s %s") ;

            for (int i = 0; i < out.data->rows; i++) {
                if(fprintf(output, "%.17g\n", (vector+out.data->cols)[i]) < 0) {
                    error_for_matrix("Error writing result entry for matrix %s %s. Skipping...") ;
                    break;
                }
            }
            log_for_matrix("Flushed matrix %s %s") ;

            fclose(output) ;
loop_free_vector :
            free(vector) ;
loop_destroy_loader:
            SCPA_CSR_DIRECT_LOADER_Destroy(&out) ;
loop_free_matrix:
            fclose(matrix) ;
loop_free_pathname:
            free(outfilename) ;
        }
        else {
            fprintf(stderr, "Error retrieving matrix data. Skipping...\n") ;
        }
    }

    fclose(matrices) ;
} 