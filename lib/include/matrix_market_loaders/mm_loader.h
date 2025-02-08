#ifndef SCPA_MMLOADER_H
#define SCPA_MMLOADER_H

#include <stdio.h>
#include "common/definitions.h"

typedef struct _SCPA_MMLOADER {

    void *data ;
    
    /**
     * This function returns the value of a matrix entry identified by row and col
     */
    int (*SCPA_MMLOADER_ReadAt)(IN struct _SCPA_MMLOADER *, IN int row, IN int col) ;

} SCPA_MMLOADER ;

int SCPA_CSR_DIRECT_LOADER_Init(IN FILE *file, OUT SCPA_MMLOADER *out) ;
int SCPA_CSR_DIRECT_LOADER_Destroy(IN SCPA_MMLOADER *loader) ;

int SCPA_HLL_DIRECT_LOADER_Init(IN FILE *file, OUT SCPA_MMLOADER *out, int hackSize) ;
int SCPA_HLL_DIRECT_LOADER_Destroy(IN SCPA_MMLOADER *loader) ;

#endif