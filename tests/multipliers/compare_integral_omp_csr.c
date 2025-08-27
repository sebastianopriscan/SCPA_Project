#include "parallel/omp/csr/scpa_csr_omp_kernel.h"
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <math.h>
#include <omp.h>

#define BUFFERSIZE 2048

#define error_for_matrix(message, ...) fprintf(stderr, message"\n", entry->d_name, ##__VA_ARGS__)
#define log_for_matrix(message) printf(message"\n", entry->d_name)

char *result_store_path = "resources/comparative/pattern_results" ;

char name_buffer[BUFFERSIZE] ;

int main(void) {

    int errors = 0;
    char *filenameStart = stpcpy(name_buffer, "resources/comparative/pattern_matrices/") ;

    DIR *matrices = opendir("resources/comparative/pattern_matrices") ;
    struct dirent *entry ;
    if (matrices == NULL) {
        perror("Error opening matrices directory") ;
        return -1 ;
    }

    while ((entry = readdir(matrices)) != NULL) {
        if (
            strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0
        ) {
            continue;
        }

        char *outfilename = malloc(strlen(result_store_path) + strlen(entry->d_name) +3) ;
        if (outfilename == NULL) {
            error_for_matrix("Error allocating output file name for matrix %s. Skipping...") ;
            errors = 1 ;
            continue ;
        }

        char *cursor = stpcpy(outfilename, result_store_path) ;
        *cursor++ = '/' ;
        cursor = stpcpy(cursor, entry->d_name) ;
        *cursor = '\0' ; 

        *stpcpy(filenameStart, entry->d_name) = '\0' ;
        FILE* matrix = fopen(name_buffer, "r");
        if (matrix == NULL) {
            error_for_matrix("Error opening matrix %s. Skipping") ;
            errors = 1 ;
            goto loop_free_pathname ;
        }

        log_for_matrix("Opened matrix %s") ;

        SCPA_MMLOADER_CSR_LOADER_DATA out ;

        if (SCPA_CSR_DIRECT_LOADER_Init(matrix, &out) != 0) {
            error_for_matrix("Error loading matrix %s in CSR format. Skipping...") ;
            errors = 1 ;
            goto loop_free_matrix ;

        }
        log_for_matrix("Loaded matrix %s in CSR format") ;

        double *vector = malloc(sizeof(double) * (out.data->rows + out.data->cols)) ;
        if (vector == NULL) {
            error_for_matrix("Error allocating vector to multiply matrix %s with. Skipping...") ;
            errors = 1 ;
            goto loop_destroy_loader ;
        }

        for (int i = 0; i < out.data->cols; i++) {
            vector[i] = (double) (i % 100) ;
        }
        memset(vector + out.data->cols, 0, sizeof(double) * out.data->rows) ;

        FILE *output = fopen(outfilename, "r") ;
        if (output == NULL) {
            error_for_matrix("Error opening output file for matrix %s. Skipping...") ;
            errors = 1 ;
            goto loop_free_vector ;
        }

        for (int i = 0; i < omp_get_num_procs(); i++) {
            omp_set_num_threads(i+1) ;
            printf("Multiplying with %d threads\n", i+1) ;
            log_for_matrix("Multiplying matrix %s") ;
            SCPA_CSR_OMP_KERNEL(&out, vector, vector + out.data->cols) ;
            log_for_matrix("Multiplied matrix %s") ;

            double num = 0 ;
            for (int i = 0; i < out.data->rows; i++) {
                if(fscanf(output, "%lg\n", &num) != 1) {
                    error_for_matrix("Error reading result entry for matrix %s. Exiting...") ;
                    errors = 1;
                    break;
                }
                if ((vector + out.data->cols)[i] != num) {
                    error_for_matrix("Error comparating entries for matrix %s, relative error for entry %d is %lg. Exiting...", i, fabs(1.0 - ((vector + out.data->cols)[i] / num))) ;
                    errors = 1;
                }
            }
            fseek(output, 0, SEEK_SET) ;
            memset(vector + out.data->cols, 0, sizeof(double) * out.data->rows) ;
        }
        log_for_matrix("Flushed matrix %s") ;

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

    closedir(matrices) ;
    return errors ;
} 