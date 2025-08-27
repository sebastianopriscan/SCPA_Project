#include "parallel/omp/csr/scpa_csr_omp_kernel.h"
#include "sparse_matrix_cache/sparse_matrix_cache.h"
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <math.h>

#ifndef MATRIX_CACHE_DIR
#define MATRIX_CACHE_DIR "/tmp/SCPA_Project"
#endif

#define FGETS_BUFSIZE 2048

#define error_for_matrix(message, ...) fprintf(stderr, message"\n", group_name, matrix_name, ##__VA_ARGS__)
#define log_for_matrix(message, ...) printf(message"\n", group_name, matrix_name, ##__VA_ARGS__)

char group_name[512] ;
char matrix_name[512] ;
char fgets_buffer[FGETS_BUFSIZE] ;
char *matrix_cache_dir = MATRIX_CACHE_DIR, *result_store_path = "resources/comparative/results" ;

int main(void) {


    int errors = 0;
    FILE *matrices = fopen("resources/comparative/matrices", "r") ;
    if (matrices == NULL) {
        perror("Error opening matrices file") ;
        return -1 ;
    }

    while (fgets(fgets_buffer, FGETS_BUFSIZE, matrices)) {
        if(sscanf(fgets_buffer, "%511s %511s\n", group_name, matrix_name) == 2) {

            char *outfilename = malloc(strlen(result_store_path) + strlen(group_name) + strlen(matrix_name) +3) ;
            if (outfilename == NULL) {
                error_for_matrix("Error allocating output file name for matrix %s %s. Exiting...") ;
                break; ;
            }

            char *cursor = stpcpy(outfilename, result_store_path) ;
            *cursor++ = '/' ;
            cursor = stpcpy(cursor, group_name) ;
            *cursor++ = '_' ;
            cursor = stpcpy(cursor, matrix_name) ;
            *cursor = '\0' ; 

            FILE* matrix ;
            SCPA_MMLOADER_CSR_LOADER_DATA out ;

            if (SCPA_SPCACHE_OpenMatrix(matrix_name, group_name, matrix_cache_dir, &matrix) == 1) {
                error_for_matrix("Error getting matrix %s %s. Exiting...") ;
                errors = 1 ;
                goto loop_free_pathname;
            }
            log_for_matrix("Opened matrix %s %s") ;

            if (SCPA_CSR_DIRECT_LOADER_Init(matrix, &out) != 0) {
                error_for_matrix("Error loading matrix %s %s in CSR format. Exiting...") ;
                errors = 1 ;
                goto loop_free_matrix ;

            }
            log_for_matrix("Loaded matrix %s %s in CSR format") ;

            double *vector = malloc(sizeof(double) * (out.data->rows+ out.data->cols)) ;
            if (vector == NULL) {
                error_for_matrix("Error allocating vector to multiply matrix %s %s with. Exiting...") ;
                errors = 1 ;
                goto loop_destroy_loader ;
            }

            for (int i = 0; i < out.data->cols; i++) {
                vector[i] = (double) (i % 100) ;
            }
            memset(vector + out.data->cols, 0, sizeof(double) * out.data->rows) ;

            FILE *output = fopen(outfilename, "r") ;
            if (output == NULL) {
                error_for_matrix("Error opening output file for matrix %s %s. Exiting...") ;
                errors = 1 ;
                goto loop_free_vector ;
            }

            log_for_matrix("Multiplying matrix %s %s") ;
            SCPA_CSR_OMP_KERNEL(&out, vector, vector + out.data->cols) ;
            log_for_matrix("Multiplied matrix %s %s") ;

            double num = 0 ;
            for (int i = 0; i < out.data->rows; i++) {
                if(fscanf(output, "%lg\n", &num) != 1) {
                    error_for_matrix("Error reading result entry for matrix %s %s. Exiting...") ;
                    errors = 1;
                    break;
                }
                if (num == 0.) {
                    if (fabs((vector + out.data->cols)[i] - num) > 0.00001) {
                        error_for_matrix("Error comparating entries for matrix %s %s, absolute error for entry %d is %lg. Exiting...", i, fabs((vector + out.data->cols)[i] - num)) ;
                        errors = 1;
                    }
                }
                else if (fabs(1.0 - ((vector + out.data->cols)[i] / num)) > 0.001) {
                    error_for_matrix("Error comparating entries for matrix %s %s, relative error for entry %d is %lg. Exiting...", i, fabs(1.0 - ((vector + out.data->cols)[i] / num))) ;
                    errors = 1;
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
            fprintf(stderr, "Error retrieving matrix data. Exiting...\n") ;
            return -1 ;
        }
    }

    fclose(matrices) ;

    return errors ;
} 