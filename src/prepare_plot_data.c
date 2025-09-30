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

    SCPA_PSQL_TIMES_ITERATOR *matrices_iterator = SCPA_PSQL_Matrices(), *times_iterator, *serial_iterator ;
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
            serial_iterator = SCPA_PSQL_TimesForConfigCSR(name.groupName, name.matName, "SERIAL", "SCPA_DIRECT_CSR_CLASS") ;
            if (times_iterator == NULL || serial_iterator == NULL) {
                fprintf(stderr, "Error getting times for kernel and matrix. Exiting...\n") ;
                SCPA_PSQL_DestroyIterator(matrices_iterator) ;
                SCPA_PSQL_Clear() ;
                return 1 ;
            }
            double serial_mean = 0. ;
            double mean = 0. ;
            double stddev = 0. ;
            double idx = 1. ;
            long time = SCPA_PSQL_NextTime(times_iterator) ;
            long serial_time = SCPA_PSQL_NextTime(serial_iterator) ;
            while(time >= 0) {
                stddev += (time-mean)*(time-mean)*(idx-1)/idx ;
                mean += (time-mean)/idx ;

                idx += 1. ;
                time = SCPA_PSQL_NextTime(times_iterator) ;
            }
            idx = 1. ;
            while(serial_time >= 0) {
                serial_mean += (serial_time-serial_mean)/idx ;

                idx += 1. ;
                serial_time = SCPA_PSQL_NextTime(serial_iterator) ;
            }
            fprintf(out, "%d %.17g %.17g %.17g %s_%s\n", name.nzs, 2. * ((double) name.nzs) / mean, stddev, serial_mean / mean, name.groupName, name.matName) ;
        }
        fclose(out) ;
        kernel_name = strtok(NULL, ",") ;
        SCPA_PSQL_ResetIterator(matrices_iterator) ;
    }
}