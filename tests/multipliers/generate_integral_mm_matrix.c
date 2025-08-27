#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define OUT_DIR "resources/comparative/pattern_matrices/"
#define OUT_DIR_LEN strlen(OUT_DIR) +1
 
#define strtol_argv(var, idx, endptr) \
    long var = strtol(argv[idx], &endptr, 10) ; \
    if (*endptr != '\0' || *argv[idx] == '\0' || errno == ERANGE || errno == EINVAL) { \
        fprintf(stderr, #var" must be a number\n") ; \
        return -1 ; \
    } \

typedef struct _coord {
    int row, col ;
    struct _coord *next ;
} coord ;

typedef struct _coord_anchor {
    coord *start ;
    coord *end ;
} coord_anchor ;

coord_anchor anchor = {
    .start = NULL ,
    .end = NULL 
} ;

static inline int append_coord(int row, int col) {
    coord *newcoord = malloc(sizeof(coord)) ;
    if (newcoord == NULL) {
        return -1 ;
    }
    newcoord->col = col ;
    newcoord->row = row ;
    newcoord->next = NULL ;
    if (anchor.start == NULL) {
        anchor.start = newcoord ;
        anchor.end = newcoord ;
    } else {
        anchor.end->next = newcoord ;
        anchor.end = newcoord ;
    }
    return 0 ;
}

static inline int search_coord(int row, int col) {
    coord *item = anchor.start ;
    while(item != NULL) {
        if (item->row == row && item->col == col) return 1 ;
        item = item->next ;
    }
    return 0 ;
}

static inline int destroy_list() {
    coord *next, *curr = anchor.start ;

    while(curr != NULL) {
        next = curr->next ;
        free(curr) ;
        curr = next ;
    }
}

int main(int argc, char **argv) {

    if (argc != 5) {
        fprintf(stderr, "Usage: progname rows cols nzs filename\n") ;
        return -1 ;
    }

    char *endptr ;
    strtol_argv(rows, 1, endptr)
    strtol_argv(cols, 2, endptr)
    strtol_argv(nzs, 3, endptr)

    char *outPath = malloc(OUT_DIR_LEN + strlen(argv[4])) ;
    if (outPath == NULL) {
        fprintf(stderr, "Error allocating buffer for pathName\n") ;
        return -1 ;
    }
    char *ptr = stpcpy(outPath, OUT_DIR) ;
    ptr = stpcpy(ptr, argv[4]) ;
    *ptr = '\0' ;

    FILE *outFile = fopen(outPath, "w+") ;
    if (outFile == NULL) {
        fprintf(stderr, "Error creating output file\n") ;
        free(outPath) ;
        return -1 ;
    }

    fprintf(
        outFile,
        "%%%%MatrixMarket matrix coordinate real general\n"
    ) ;

    fprintf(
        outFile,
        "  %ld  %ld  %ld\n",
        rows, cols, nzs
    ) ;

    for(int i = 0; i < nzs; i++) {
        int row ;
        int col ;
        do {
            row = rand() % rows ;
            col = rand() % cols ;
        } while(search_coord(row, col)) ;

        double nz = rand() % 10 ;
        fprintf(
            outFile,
            "    %d    %d    %.17g\n",
            row+1, col+1, nz+1
        ) ;
        append_coord(row, col) ;
    }

    destroy_list() ;
    free(outPath) ;
    fclose(outFile) ;
    return 0 ;
}