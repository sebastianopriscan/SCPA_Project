#include <string.h>
#include <stdlib.h>

const char *expected = "https://suitesparse-collection-website.herokuapp.com/MM/ACUSIM/Pres_Poisson.tar.gz" ;

extern char *(*scpa_spcache_create_url_ptr)(char *, char *) ;

int main(void) {

    char *actual = scpa_spcache_create_url_ptr("Pres_Poisson", "ACUSIM") ;
    if (actual == NULL) return 1 ;
    int retval = strcmp(expected, actual) ;
    free(actual) ;
    return retval ;
}