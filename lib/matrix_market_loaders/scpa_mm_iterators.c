#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "matrix_market_loaders/mm_iterators.h"

typedef struct _SYMMETRY_STATE {
    int symmetry ;
    SCPA_MM_ENTRY last_read ;
} SYMMETRY_STATE ;

static SCPA_MM_ENTRY *generic_next(SCPA_MM_ITERATOR *this) {
    SCPA_MM_ENTRY *entry = malloc(sizeof(SCPA_MM_ENTRY)) ;
    if (entry == NULL) {
        return NULL ;
    }

    if (fscanf(this->file, "%d %d %lg", &entry->coordx, &entry->coordy, &entry->value) != 3) {
        errno ;
        entry->coordx = -1 ;
        return entry ;
    }

    return entry ;
}

static SCPA_MM_ENTRY *symmetric_next(SCPA_MM_ITERATOR *this) {
    SCPA_MM_ENTRY *entry = malloc(sizeof(SCPA_MM_ENTRY)) ;
    if (entry == NULL) {
        return NULL ;
    }

    if (((SYMMETRY_STATE *)(this->state))->symmetry) {
        ((SYMMETRY_STATE *)(this->state))->symmetry = 0 ;
        memcpy(entry, &((SYMMETRY_STATE *)(this->state))->last_read, sizeof(SCPA_MM_ENTRY)) ;
        return entry ;
    } else {
        if (fscanf(this->file, "%d %d %lg", &entry->coordx, &entry->coordy, &entry->value) != 3) {
            errno = EBADF ;
            free(entry) ;
            return NULL ;
        }

        ((SYMMETRY_STATE *)(this->state))->symmetry = 1 ;
        memcpy(&((SYMMETRY_STATE *)(this->state))->last_read, entry, sizeof(SCPA_MM_ENTRY)) ;
    }

    return entry ;
}

static SCPA_MM_ENTRY *skew_next(SCPA_MM_ITERATOR *this) {
    SCPA_MM_ENTRY *entry = malloc(sizeof(SCPA_MM_ENTRY)) ;
    if (entry == NULL) {
        return NULL ;
    }

    if (((SYMMETRY_STATE *)(this->state))->symmetry) {
        ((SYMMETRY_STATE *)(this->state))->symmetry = 0 ;
        memcpy(entry, &((SYMMETRY_STATE *)(this->state))->last_read, sizeof(SCPA_MM_ENTRY)) ;
        entry->value = -entry->value ;
        return entry ;
    } else {
        if (fscanf(this->file, "%d %d %lg", &entry->coordx, &entry->coordy, &entry->value) != 3) {
            errno = EBADF ;
            free(entry) ;
            return NULL ;
        }

        ((SYMMETRY_STATE *)(this->state))->symmetry = 1 ;
        memcpy(&((SYMMETRY_STATE *)(this->state))->last_read, entry, sizeof(SCPA_MM_ENTRY)) ;
    }

    return entry ;
}

SCPA_MM_ITERATOR *SCPA_MM_ITERATOR_Create(IN FILE *file) {

    MM_typecode matcode ;

    if(mm_read_banner(file, &matcode)) {
        errno = EBADF ;
        return NULL ;
    }

    if (!mm_is_matrix(matcode) || !mm_is_real(matcode) || !mm_is_sparse(matcode)) {
        errno = EOPNOTSUPP ;
        return NULL ;
    }

    SCPA_MM_ITERATOR *iterator = malloc(sizeof(SCPA_MM_ITERATOR) + sizeof(SYMMETRY_STATE)) ;
    if (iterator == NULL) {
        return NULL ;
    }

    if(mm_read_mtx_crd_size(file, &iterator->rows, &iterator->cols, &iterator->nz)) {
        errno = EBADF ;
        free(iterator) ;
        return NULL ;
    }
    iterator->state = iterator + sizeof(SCPA_MM_ITERATOR) ;
    ((SYMMETRY_STATE *)(iterator->state))->symmetry = 0 ;


    switch(matcode[3]) {
        case 'K' :
            iterator->next = skew_next ;
            break ;
        case 'S' :
            iterator->next = symmetric_next ;
            break ;
        case 'G' :
            iterator->next = generic_next ;
            break ;
        default :
            free(iterator) ;
            return NULL ;
    }

    iterator->file = file ;

    return iterator ;
}