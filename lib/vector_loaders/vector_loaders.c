#include <stdio.h>
#include <stdlib.h>

#include "vector_loaders/vector_loaders.h"

double *SCPA_VECTOR_LoadFromFile(char *path) {

    FILE *file = fopen(path, "r") ;
    if (file == NULL) return NULL ;

    int size, scanfRet ;
    scanfRet = fscanf(file, "%d\n", &size) ;
    if (scanfRet != 1 || scanfRet == EOF) {
        fclose(file) ;
        return NULL ;    
    }

    double *retVal = malloc(sizeof(double) * size) ;
    if (retVal == NULL) {
        fclose(file) ;
        return NULL ;
    }

    int i = 0 ;
    do {

        scanfRet = fscanf(file, "%le\n", retVal + i) ;
        if (scanfRet != 1 || scanfRet == EOF) {
            fclose(file) ;
            free(retVal) ;
            return NULL ;    
        }
        i++ ;

    } while (i < size) ;

    fclose(file) ;

    return retVal ;
}

double *SCPA_VECTOR_LoadRandom(int size, unsigned int seed) {

    srand(seed) ;

    double *retVal = malloc(sizeof(double) * size) ;
    if ( retVal == NULL) {
        return NULL ;
    }

    for (int i = 0; i < size; i++)
        retVal[i] = ((double) rand()) / RAND_MAX * RAND_MAX ;

    return retVal ;
}

double *SCPA_VECTOR_LoadRandomPattern(int size, unsigned int seed) {

    srand(seed) ;

    double *retVal = malloc(sizeof(double) * size) ;
    if ( retVal == NULL) {
        return NULL ;
    }

    for (int i = 0; i < size; i++)
        retVal[i] = rand() > (RAND_MAX / 2) ? 0. : 1. ;

    return retVal ;
}