#include <sys/stat.h>
#include <string.h>
#include <errno.h>

int SCPA_mkdir_recurse(char *path) {

    char *start = path ;
    if (path[0] == '/') start++ ;

    char *token = strtok(start, "/") ;

    if (strcmp(start, "..") == 0) return -1 ;

    if (strcmp(start, ".") != 0) {
        if (mkdir(path, 0771)) {
            if (errno != EEXIST) {
                return -1 ;
            }
        }
    }

    while (1) {
        token = strtok(NULL, "/") ;
        if (token == NULL) break;
        *(token -1) = '/' ;
        if (strcmp(token, "..") == 0 || strcmp(token, ".") == 0) continue ;

        if (mkdir(path, 0771)) {
            if (errno != EEXIST) {
                return -1 ;
            }
        }
    }

}