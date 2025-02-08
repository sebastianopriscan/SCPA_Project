#ifndef MM_ITERATORS_H
#define MM_ITERATORS_H

#include <stdio.h>
#include "matrix_market_io/mmio.h"
#include "common/definitions.h"

typedef struct _SCPA_MM_ENTRY { 
    int coordx ; 
    int coordy ; 
    double value ;
} SCPA_MM_ENTRY ; 

typedef struct _SCPA_MM_ITERATOR {
    FILE *file ;
    void *state ;
    int rows ;
    int cols ;
    int nz ;
    MALLOCD SCPA_MM_ENTRY *(*next)(struct _SCPA_MM_ITERATOR *) ;
} SCPA_MM_ITERATOR ;

void SCPA_MM_ITERATOR_Reset(IN SCPA_MM_ITERATOR *) ;

MALLOCD SCPA_MM_ITERATOR *SCPA_MM_ITERATOR_Create(IN FILE *file) ;

#endif