#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "os_wrap/fork_exec.h"

#define TGZ ".tar.gz"
#define TGZLEN 8

#define PWD "PWD="
#define PWDLEN 5

#define MTX ".mtx"
#define MTXLEN 6

static inline MALLOCD char *build_pwd_env(char *workDir) {

    char *basePWD = getenv("PWD") ;
    if (basePWD == NULL) return NULL ;
    char *newPWD = malloc(strlen(basePWD) + strlen(workDir) +2) ;
    if (newPWD == NULL) return NULL ;

    char *cursor = stpcpy(newPWD, basePWD) ;
    *cursor++ = '/' ;
    cursor = stpcpy(cursor, newPWD) ;
    *cursor = '\0' ;

    return newPWD ;
}

int wget_score(char *url, char *matrixName, char *workDir) {

    //Prepare parameters

    //Filename .tar.gz
    char *tgzname = malloc(strlen(matrixName) + TGZLEN) ;
    if (tgzname == NULL) return 1 ;
    char *cursor = stpcpy(tgzname, matrixName) ;
    cursor = stpcpy(cursor, TGZ) ;
    *cursor = '\0' ;

    //PWD environment variable
    char *newPWD = workDir;
    if (workDir[0] != '/') newPWD = build_pwd_env(workDir) ;

    if (newPWD == NULL) {
        free(tgzname) ;
        return 1 ;
    }

    char *pwdenv = malloc(strlen(newPWD) + PWDLEN) ;
    if (pwdenv == NULL) {
        free(tgzname) ;
        if (workDir[0] != '/') free(newPWD) ;
        return 1 ;
    }
    cursor = stpcpy(pwdenv, PWD) ;
    cursor = stpcpy(cursor, newPWD) ;
    *cursor = '\0' ;

    if (workDir[0] != '/') free(newPWD) ;

    char *env[2] = {pwdenv, NULL} ;

    int retval = 0 ;

    //Invoke wget
    char *wget_args[5] = {"wget", url, "-O", tgzname, NULL} ;

    if (SCPA_fork_exec_wait(wget_args, env)) {
        retval = 1 ;
        goto terminate ;
    }

    //Invoke tar
    char *tar_args[4] = {"tar", "-xzf", tgzname, NULL} ;

    if (SCPA_fork_exec_wait(tar_args, env)) {
        retval = 1 ;
        goto terminate ;
    }

    //Move .mtx file

    char *uncompressed_file_name_src = malloc(strlen(matrixName)*2 + MTXLEN) ;
    if (uncompressed_file_name_src == NULL) {
        retval = 1 ;
        goto terminate ;
    }
    cursor = stpcpy(cursor, matrixName) ;
    *cursor++ = '/' ;
    cursor = stpcpy(cursor, matrixName) ;
    cursor = stpcpy(cursor, MTX) ;

    char *uncompressed_file_name_dest = malloc(strlen(matrixName) + MTXLEN) ;
    if (uncompressed_file_name_dest == NULL) {
        free(uncompressed_file_name_src) ;
        retval = 1 ;
        goto terminate ;
    }
    cursor = stpcpy(cursor, matrixName) ;
    cursor = stpcpy(cursor, MTX) ;

    char *mv_args[4] = {"mv", uncompressed_file_name_src, uncompressed_file_name_dest, NULL} ;

    if (SCPA_fork_exec_wait(mv_args, env)) {
        retval = 1 ;
        goto free_uncompressed_terminate ;
    }

    //Remove dir with original .mtx
    char *rm_mtx_args[4] = {"rm", "-rf", matrixName, NULL} ;

    if (SCPA_fork_exec_wait(rm_mtx_args, env)) {
        retval = 1 ;
        goto free_uncompressed_terminate ;
    }

    //Remove tar.gz
    char *rm_tgz_args[4] = {"rm", "-rf", tgzname, NULL} ;

    if (SCPA_fork_exec_wait(rm_tgz_args, env)) {
        retval = 1 ;
        goto free_uncompressed_terminate ;
    }

    //Final rename
    char *rename_mv_args[4] = {"mv", tgzname, matrixName, NULL} ;

    if (SCPA_fork_exec_wait(rename_mv_args, env)) {
        retval = 1 ;
        goto free_uncompressed_terminate ;
    }

free_uncompressed_terminate :

    free(uncompressed_file_name_src) ;
    free(uncompressed_file_name_dest) ;

terminate :

    free(tgzname) ;
    free(pwdenv) ;

    return retval ;
}