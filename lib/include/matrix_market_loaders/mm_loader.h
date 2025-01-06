#ifndef SCPA_MMLOADER_H
#define SCPA_MMLOADER_H

#include <stdio.h>
#include "common/definitions.h"

typedef struct _SCPA_MMLOADER {

    void *data ;
    
    /**
     * This function returns the value of a matrix entry identified by row and col
     */
    int (*SCPA_MMLOADER_ReadAt)(IN SCPA_MMLOADER *, IN int row, IN int col) ;

} SCPA_MMLOADER ;

#endif