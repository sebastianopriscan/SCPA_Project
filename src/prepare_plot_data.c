#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "psql/psql.h"

char nameBuffer[4096] ;

int main(int argc, char **argv) {

    if (argc != 2) {
        fprintf(stderr, "Usage: ./prog kernel[,kernel]*\n") ;
        return 1 ;
    }

    char *password = getenv("PGPASSWORD") ;
    if (!password) {
        fprintf(stderr, "PGPASSWORD environment variable not set\n") ;
        return 1 ;
    }

    SCPA_PSQL_Setup(
        "localhost",
        "5432",
        "scpa",
        "scpa",
        password
    ) ;

    SCPA_PSQL_TIMES_ITERATOR *matrices_iterator = SCPA_PSQL_Matrices(), *times_iterator ;
    if (matrices_iterator == NULL) {
        fprintf(stderr, "Error getting matrices names. Exiting...\n") ;
        SCPA_PSQL_Clear() ;
        return 1 ;
    }

    SCPA_MatrixName name ;

    char *kernel_name = strtok(argv[1], ",");

    while(kernel_name != NULL) {

        sprintf(nameBuffer, "%s.dat", kernel_name) ;
        FILE *out = fopen(nameBuffer, "w+") ;
        if (out == NULL) {
            fprintf(stderr, "Error opening out file. Exiting...\n") ;
            SCPA_PSQL_Clear() ;
            return 1 ;
        }

        while(!SCPA_PSQL_NextMatrix(matrices_iterator, &name)) {
            times_iterator = SCPA_PSQL_TimesForConfigCSR(name.groupName, name.matName, kernel_name, "SCPA_DIRECT_CSR_CLASS") ;
            if (times_iterator == NULL) {
                fprintf(stderr, "Error getting times for kernel and matrix. Exiting...\n") ;
                SCPA_PSQL_DestroyIterator(matrices_iterator) ;
                SCPA_PSQL_Clear() ;
                return 1 ;
            }
            double mean = 0. ;
            double stddev = 0. ;
            double idx = 1. ;
            int time = SCPA_PSQL_NextTime(times_iterator) ;
            while(time >= 0) {
                stddev += (time-mean)*(time-mean)*(idx-1)/idx ;
                mean += (time-mean)/idx ;

                idx += 1. ;
                time = SCPA_PSQL_NextTime(times_iterator) ;
            }
            fprintf(out, "%d %.17g %.17g %s_%s\n", name.nzs, 2. * ((double) name.nzs) / mean, stddev, name.groupName, name.matName) ;
        }
        fclose(out) ;
        kernel_name = strtok(NULL, ",") ;
        SCPA_PSQL_ResetIterator(matrices_iterator) ;
    }
}