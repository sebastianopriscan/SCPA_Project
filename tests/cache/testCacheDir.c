#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

const char *expected = "/tmp/exampleDir/anotherExample/../anotherExample/./ACUSIM" ;

extern char *(*scpa_spcache_create_cache_dir)(char *, char *) ;

static inline int test() {
    char *actual = scpa_spcache_create_cache_dir("ACUSIM", "/tmp/exampleDir/anotherExample/../anotherExample/.") ;
    if (actual == NULL) return 1 ;
    int retval = strcmp(expected, actual) ;
    free(actual) ;
    if (retval) return 1 ;

    struct stat ignored ;
    if (stat("/tmp/exampleDir/anotherExample/ACUSIM", &ignored)) return 1 ;
}

static inline void after() {
    if (rmdir("/tmp/exampleDir/anotherExample/ACUSIM")) perror("Error deleting ACUSIM directory: ") ;
    if (rmdir("/tmp/exampleDir/anotherExample")) perror("Error deleting anotherExample directory: ") ;
    if (rmdir("/tmp/exampleDir")) perror("Error deleting exampleDir directory: ") ;
}

int main(void) {

    int retval = test() ;

    after() ;

    return retval ;
}