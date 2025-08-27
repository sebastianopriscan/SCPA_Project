#include "serial/csr/scpa_csr_serial_kernel.h"
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

#define BUFFERSIZE 2048

#define error_for_matrix(message) fprintf(stderr, message"\n", entry->d_name)
#define log_for_matrix(message) printf(message"\n", entry->d_name)

char name_buffer[BUFFERSIZE] ;

int main(int argc, char **argv) {

    if (argc != 2) {
        fprintf(stderr, "Usage: ./progName result_store_path") ;
        return -1 ;
    }

    char *result_store_path = argv[1] ;

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

        char *outfilename = malloc(strlen(argv[1]) + strlen(entry->d_name) +3) ;
        if (outfilename == NULL) {
            error_for_matrix("Error allocating output file name for matrix %s. Skipping...") ;
            continue ;
        }

        char *cursor = stpcpy(outfilename, argv[1]) ;
        *cursor++ = '/' ;
        cursor = stpcpy(cursor, entry->d_name) ;
        *cursor = '\0' ; 

        struct stat ignored ;
        if (stat(outfilename, &ignored) == 0) {
            log_for_matrix("Data for matrix %s already generated. Skipping...") ;
            goto loop_free_pathname ;
        }

        *stpcpy(filenameStart, entry->d_name) = '\0' ;
        FILE* matrix = fopen(name_buffer, "r");
        if (matrix == NULL) {
            error_for_matrix("Error opening matrix %s. Skipping") ;
            goto loop_free_pathname ;
        }

        log_for_matrix("Opened matrix %s") ;

        SCPA_MMLOADER_CSR_LOADER_DATA out ;

        if (SCPA_CSR_DIRECT_LOADER_Init(matrix, &out) != 0) {
            error_for_matrix("Error loading matrix %s in CSR format. Skipping...") ;
            goto loop_free_matrix ;

        }
        log_for_matrix("Loaded matrix %s in CSR format") ;

        double *vector = malloc(sizeof(double) * (out.data->rows + out.data->cols)) ;
        if (vector == NULL) {
            error_for_matrix("Error allocating vector to multiply matrix %s with. Skipping...") ;
            goto loop_destroy_loader ;
        }

        for (int i = 0; i < out.data->cols; i++) {
            vector[i] = (double) (i % 100) ;
        }
        memset(vector + out.data->cols, 0, sizeof(double) * out.data->rows) ;

        FILE *output = fopen(outfilename, "w+") ;
        if (output == NULL) {
            error_for_matrix("Error opening output file for matrix %s. Skipping...") ;
            goto loop_free_vector ;
        }

        log_for_matrix("Multiplying matrix %s") ;
        SCPA_CSR_SERIAL_KERNEL(&out, vector, vector + out.data->cols) ;
        log_for_matrix("Multiplied matrix %s") ;

        for (int i = 0; i < out.data->rows; i++) {
            if(fprintf(output, "%.17g\n", (vector+out.data->cols)[i]) < 0) {
                error_for_matrix("Error writing result entry for matrix %s. Skipping...") ;
                break;
            }
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
} 