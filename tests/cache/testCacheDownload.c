#include <stdio.h>
#include "sparse_matrix_cache/sparse_matrix_cache.h"
#include "os_wrap/fork_exec.h"

#define MATNAME "1138_bus"
#define MATGROUP "HB"

char *dir = "/tmp/exampleDir/anotherExample/../anotherExample" ;

static inline void after() {
    char *rmCmd[] = {"/bin/rm", "-rf", "/tmp/exampleDir", NULL} ;
    if (SCPA_fork_exec_wait(rmCmd, ".")) {
        perror("Error removing cache dir: ") ;
    }
}

static int test(void) {

    FILE *out ;

    if (SCPA_SPCACHE_OpenMatrix(MATNAME, MATGROUP, dir, &out)) {
        fclose(out) ;
        return -1 ;
    }

    fclose(out) ;

    //If no one touches the cache directory, this should be a cache hit
    if (SCPA_SPCACHE_OpenMatrix(MATNAME, MATGROUP, dir, &out)) {
        fclose(out) ;
        return -1 ;
    }

    fclose(out) ;
    return 0 ;
}

int main(void) {

    int retval = test() ;

    after() ;

    return retval ;
}