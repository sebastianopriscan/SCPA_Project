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
        int temp = entry->coordx ;
        entry->coordx = entry->coordy ;
        entry->coordy = temp ;
        return entry ;
    } else {
        if (fscanf(this->file, "%d %d %lg", &entry->coordx, &entry->coordy, &entry->value) != 3) {
            entry->coordx = -1 ;
            return entry ;
        }

        if (entry->coordx != entry->coordy) {
            ((SYMMETRY_STATE *)(this->state))->symmetry = 1 ;
            memcpy(&((SYMMETRY_STATE *)(this->state))->last_read, entry, sizeof(SCPA_MM_ENTRY)) ;
        }
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
        int temp = entry->coordx ;
        entry->coordx = entry->coordy ;
        entry->coordy = temp ;
        return entry ;
    } else {
        if (fscanf(this->file, "%d %d %lg", &entry->coordx, &entry->coordy, &entry->value) != 3) {
            entry->coordx = -1 ;
            return entry ;
        }

        if (entry->coordx != entry->coordy) {
            ((SYMMETRY_STATE *)(this->state))->symmetry = 1 ;
            memcpy(&((SYMMETRY_STATE *)(this->state))->last_read, entry, sizeof(SCPA_MM_ENTRY)) ;
        }
    }

    return entry ;
}

static SCPA_MM_ENTRY *generic_next_pattern(SCPA_MM_ITERATOR *this) {
    SCPA_MM_ENTRY *entry = malloc(sizeof(SCPA_MM_ENTRY)) ;
    if (entry == NULL) {
        return NULL ;
    }

    if (fscanf(this->file, "%d %d", &entry->coordx, &entry->coordy) != 2) {
        entry->coordx = -1 ;
        return entry ;
    }
    entry->value = 1.0 ;

    return entry ;
}

static SCPA_MM_ENTRY *symmetric_next_pattern(SCPA_MM_ITERATOR *this) {
    SCPA_MM_ENTRY *entry = malloc(sizeof(SCPA_MM_ENTRY)) ;
    if (entry == NULL) {
        return NULL ;
    }

    if (((SYMMETRY_STATE *)(this->state))->symmetry) {
        ((SYMMETRY_STATE *)(this->state))->symmetry = 0 ;
        memcpy(entry, &((SYMMETRY_STATE *)(this->state))->last_read, sizeof(SCPA_MM_ENTRY)) ;
        int temp = entry->coordx ;
        entry->coordx = entry->coordy ;
        entry->coordy = temp ;
        return entry ;
    } else {
        if (fscanf(this->file, "%d %d", &entry->coordx, &entry->coordy) != 2) {
            entry->coordx = -1 ;
            return entry ;
        }
        entry->value = 1.0 ;

        if (entry->coordx != entry->coordy) {
            ((SYMMETRY_STATE *)(this->state))->symmetry = 1 ;
            memcpy(&((SYMMETRY_STATE *)(this->state))->last_read, entry, sizeof(SCPA_MM_ENTRY)) ;
        }
    }

    return entry ;
}

static SCPA_MM_ENTRY *skew_next_pattern(SCPA_MM_ITERATOR *this) {
    SCPA_MM_ENTRY *entry = malloc(sizeof(SCPA_MM_ENTRY)) ;
    if (entry == NULL) {
        return NULL ;
    }

    if (((SYMMETRY_STATE *)(this->state))->symmetry) {
        ((SYMMETRY_STATE *)(this->state))->symmetry = 0 ;
        memcpy(entry, &((SYMMETRY_STATE *)(this->state))->last_read, sizeof(SCPA_MM_ENTRY)) ;
        entry->value = -entry->value ;
        int temp = entry->coordx ;
        entry->coordx = entry->coordy ;
        entry->coordy = temp ;
        return entry ;
    } else {
        if (fscanf(this->file, "%d %d", &entry->coordx, &entry->coordy) != 2) {
            entry->coordx = -1 ;
            return entry ;
        }
        entry->value = 1.0 ;

        if (entry->coordx != entry->coordy) {
            ((SYMMETRY_STATE *)(this->state))->symmetry = 1 ;
            memcpy(&((SYMMETRY_STATE *)(this->state))->last_read, entry, sizeof(SCPA_MM_ENTRY)) ;
        }
    }

    return entry ;
}

static int readNzs(FILE *file, int isPattern) {
    int nzs = 0 ;
    int row, col ;
    double dummy ;
    MM_typecode matcode ;

    while(
        !isPattern ?
            fscanf(file, "%d %d %lg", &row, &col, &dummy) == 3 :
            fscanf(file, "%d %d", &row, &col) == 2
    )
        nzs += row != col ? 2 : 1 ;
    
    fseek(file, 0, SEEK_SET) ;
    mm_read_banner(file, &matcode) ;
    mm_read_mtx_crd_size(file, &row, &col, &col) ;

    return nzs ;
}

void SCPA_MM_ITERATOR_Reset(IN SCPA_MM_ITERATOR *iterator) {
    int row, col ;
    MM_typecode matcode ;
    fseek(iterator->file, 0, SEEK_SET) ;
    mm_read_banner(iterator->file, &matcode) ;
    mm_read_mtx_crd_size(iterator->file, &row, &col, &col) ;
}

SCPA_MM_ITERATOR *SCPA_MM_ITERATOR_Create(IN FILE *file) {

    MM_typecode matcode ;

    if(mm_read_banner(file, &matcode)) {
        errno = EBADF ;
        return NULL ;
    }

    if (!mm_is_matrix(matcode) || mm_is_complex(matcode) || !mm_is_sparse(matcode)) {
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
    iterator->state = (void *)iterator + sizeof(SCPA_MM_ITERATOR) ;
    ((SYMMETRY_STATE *)(iterator->state))->symmetry = 0 ;


    switch(matcode[3]) {
        case 'K' :
            iterator->nz = readNzs(file, mm_is_pattern(matcode)) ;
            iterator->next = mm_is_pattern(matcode) ? skew_next_pattern : skew_next ;
            break ;
        case 'S' :
            iterator->nz = readNzs(file, mm_is_pattern(matcode)) ;
            iterator->next = mm_is_pattern(matcode) ? symmetric_next_pattern : symmetric_next ;
            break ;
        case 'G' :
            iterator->next = mm_is_pattern(matcode) ? generic_next_pattern : generic_next ;
            break ;
        default :
            free(iterator) ;
            return NULL ;
    }

    iterator->file = file ;

    return iterator ;
}