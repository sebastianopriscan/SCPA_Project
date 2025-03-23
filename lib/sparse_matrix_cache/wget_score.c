#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "os_wrap/fork_exec.h"

#define TGZ ".tar.gz"
#define TGZLEN 8


#define MTX ".mtx"
#define MTXLEN 6


int wget_score(char *url, char *matrixName, char *workDir) {

    //Prepare parameters

    //Filename .tar.gz
    char *tgzname = malloc(strlen(matrixName) + TGZLEN) ;
    if (tgzname == NULL) return 1 ;
    char *cursor = stpcpy(tgzname, matrixName) ;
    cursor = stpcpy(cursor, TGZ) ;
    *cursor = '\0' ;

    int retval = 0 ;

    //Invoke wget
    char *wget_args[5] = {"/bin/wget", url, "-O", tgzname, NULL} ;

    if (SCPA_fork_exec_wait(wget_args, workDir)) {
        retval = 1 ;
        goto terminate ;
    }

    //Invoke tar
    char *tar_args[4] = {"/bin/tar", "-xzf", tgzname, NULL} ;

    if (SCPA_fork_exec_wait(tar_args, workDir)) {
        retval = 1 ;
        goto terminate;
    }

    //Move .mtx file

    char *uncompressed_file_name_src = malloc(strlen(matrixName)*2 + MTXLEN) ;
    if (uncompressed_file_name_src == NULL) {
        retval = 1 ;
        goto terminate ;
    }
    cursor = stpcpy(uncompressed_file_name_src, matrixName) ;
    *cursor++ = '/' ;
    cursor = stpcpy(cursor, matrixName) ;
    cursor = stpcpy(cursor, MTX) ;

    char *uncompressed_file_name_dest = malloc(strlen(matrixName) + MTXLEN) ;
    if (uncompressed_file_name_dest == NULL) {
        free(uncompressed_file_name_src) ;
        retval = 1 ;
        goto terminate ;
    }
    cursor = stpcpy(uncompressed_file_name_dest, matrixName) ;
    cursor = stpcpy(cursor, MTX) ;

    char *mv_args[4] = {"/bin/mv", uncompressed_file_name_src, uncompressed_file_name_dest, NULL} ;

    if (SCPA_fork_exec_wait(mv_args, workDir)) {
        retval = 1 ;
        goto free_uncompressed_terminate ;
    }

    //Remove dir with original .mtx
    char *rm_mtx_args[4] = {"/bin/rm", "-rf", matrixName, NULL} ;

    if (SCPA_fork_exec_wait(rm_mtx_args, workDir)) {
        retval = 1 ;
        goto free_uncompressed_terminate ;
    }

    //Remove tar.gz
    char *rm_tgz_args[4] = {"/bin/rm", "-rf", tgzname, NULL} ;

    if (SCPA_fork_exec_wait(rm_tgz_args, workDir)) {
        retval = 1 ;
        goto free_uncompressed_terminate ;
    }

    //Final rename
    char *rename_mv_args[4] = {"/bin/mv", uncompressed_file_name_dest, matrixName, NULL} ;

    if (SCPA_fork_exec_wait(rename_mv_args, workDir)) {
        retval = 1 ;
        goto free_uncompressed_terminate ;
    }

free_uncompressed_terminate :

    free(uncompressed_file_name_src) ;
    free(uncompressed_file_name_dest) ;

terminate :

    free(tgzname) ;

    return retval ;
}