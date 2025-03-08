#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

const char *expected = "exampleDir/ACUSIM" ;

extern char *(*scpa_spcache_create_cache_dir)(char *, char *) ;

static inline int test() {
    char *actual = scpa_spcache_create_cache_dir("ACUSIM", "exampleDir") ;
    if (actual == NULL) return 1 ;
    int retval = strcmp(expected, actual) ;
    free(actual) ;
    if (retval) return 1 ;

    struct stat ignored ;
    if (stat("exampleDir/ACUSIM", &ignored)) return 1 ;
}

static inline void after() {
    if (rmdir("exampleDir/ACUSIM")) perror("Error deleting ACUSIM directory: ") ;
    if (rmdir("exampleDir")) perror("Error deleting exampleDir directory: ") ;
}

int main(void) {

    int retval = test() ;

    after() ;

    return retval ;
}