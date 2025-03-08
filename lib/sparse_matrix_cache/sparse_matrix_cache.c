#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include "sparse_matrix_cache/sparse_matrix_cache.h"

/**
 * This is the length of the common part of the URL to invoke in order to get the matrix plus \0
 */
#define BASE_URL_LENGTH 65

const char *URL_ROOT = "https://suitesparse-collection-website.herokuapp.com/MM/" ;
const char *URL_END = ".tar.gz" ;


static inline MALLOCD char *createURL(IN char *matrixName, IN char *matrixGroup) {

    char *retVal = malloc(BASE_URL_LENGTH + strlen(matrixGroup) + strlen(matrixName)) ;
    if (retVal == NULL) return NULL ;

    char *cursor ;
    cursor = stpcpy(retVal, URL_ROOT) ;
    cursor = stpcpy(cursor, matrixGroup) ;
    *cursor++ = '/' ;
    cursor = stpcpy(cursor, matrixName) ;
    cursor = stpcpy(cursor, URL_END) ;
    *cursor = '\0' ;

    return retVal ;
}

static inline MALLOCD char *cache_dir(IN char *matrixGroup, IN char *cache_root_dir) {

    char *allocd = malloc(strlen(cache_root_dir) + strlen(matrixGroup) +2) ;
    if (allocd == NULL) {
        return NULL ;
    }
    char *cursor ;
    cursor = stpcpy(allocd, cache_root_dir) ;
    *cursor = '\0' ;

    if(mkdir(allocd, 0771)) {
        if (errno != EEXIST) {
            free(allocd) ;
            return NULL ;
        }
    }

    *cursor++ = '/' ;
    cursor = stpcpy(cursor, matrixGroup) ;
    *cursor = '\0' ;

    if(mkdir(allocd, 0771)) {
        if (errno != EEXIST) {
            free(allocd) ;
            return NULL ;
        }
    }

    return allocd ;
}

static inline MALLOCD char *build_path_name(IN char *matrixName, IN char *matrixGroup, IN char *cache_root_dir) {

    char *allocd = malloc(strlen(cache_root_dir) + strlen(matrixGroup) + strlen(matrixName) +3) ;
    if (allocd == NULL) return NULL ;

    char *cursor ;
    cursor = stpcpy(allocd, cache_root_dir) ;
    *cursor++ = '/' ;
    cursor = stpcpy(cursor, matrixGroup) ;
    *cursor++ = '/' ;
    cursor = stpcpy(cursor, matrixName) ;
    *cursor = '\0' ;

    return allocd ;
}

static inline int wget_cache(IN char *target, IN char *matrixGroup, IN char *matrixName, IN char *cache_root_dir) {

    struct stat ignored ;
    if (stat(target, &ignored)) {
        if (errno != ENOENT) return 1 ;
        char *cacheDir = cache_dir(matrixGroup, cache_root_dir) ;
        if (cacheDir == NULL) return 1 ;

        int pid = fork() ;
        if (pid == -1) {
            free(cacheDir) ;
            return 1 ;
        }

        if (pid == 0) {
            int status ;
            do { wait(&status) ;} while(!WIFEXITED(status)) ;
            if (WEXITSTATUS(status) == EXIT_FAILURE) {
                free(cacheDir) ;
                return 1 ;
            }
        } else {

            char *url = createURL(matrixName, matrixGroup) ;
            if (url == NULL) exit(EXIT_FAILURE) ;

            char *env[1] = {(char *)NULL} ;
            char *arg[4] = {"./download.sh", cacheDir, url, matrixName} ;

            execve("./download.sh", arg, env) ;

            free(url) ;
            exit(EXIT_FAILURE) ;
        }

        return stat(target, &ignored) ? 1 : 0 ;

    } else return 0 ;
}

int SCPA_SPCACHE_OpenMatrix(IN char *matrixName, IN char *matrixGroup, IN char *cache_root_dir, OUT FILE **file) {

    char *target = build_path_name(matrixName, matrixGroup, cache_root_dir) ;
    if (target == NULL) return 1 ;

    if (wget_cache(target, matrixGroup, matrixName, cache_root_dir)) {
        free(target) ;
        return 1 ;
    }

    *file = fopen(target, "r") ;
    free(target) ;

    return *file == NULL ;
}


#ifdef TEST_BUILD
MALLOCD char *(*scpa_spcache_create_url_ptr)(char *, char *) = createURL ;
MALLOCD char *(*scpa_spcache_create_cache_dir)(char *, char *) = cache_dir ;
#endif