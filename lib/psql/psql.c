#include <stdlib.h>
#include <errno.h>
#include <postgresql/libpq-fe.h>

#include "psql/psql.h"

#define SELECT_TIMES_QUERY "SELECT time FROM results WHERE matrix_name=$1 AND group_name=$2 AND kernel_id=$3 AND loader=$4 ;"
#define SELECT_TIMES_PARAMS 4

#define MATRICES_NAMES_QUERY "SELECT DISTINCT group_name, matrix_name, nzs FROM results ORDER BY nzs ;"

const Oid selectOids[4] = {
    1043,
    1043,
    1043,
    16391
} ;

PGconn *connection = NULL;

typedef struct _SCPA_PSQL_TIMES_ITERATOR {
    PGresult *result ;
    int index ; 
    int limit ;
} SCPA_PSQL_TIMES_ITERATOR ;

long SCPA_PSQL_NextTime(SCPA_PSQL_TIMES_ITERATOR *iterator) {
    if (iterator->index == iterator->limit) {
        return -1 ;
    }

    char *retStr = PQgetvalue(iterator->result, iterator->index, 0) ;
    iterator->index++ ;

    char *endptr ;
    long retVal = strtol(retStr, &endptr, 10) ;
    if (*endptr != '\0' || *retStr == '\0' || errno == ERANGE || errno == EINVAL) {
        PQclear(iterator->result) ;
        free(iterator) ;
        return -1 ;
    }

    return retVal ;
}

int SCPA_PSQL_NextMatrix(SCPA_PSQL_TIMES_ITERATOR *iterator, SCPA_MatrixName *name) {
    if (iterator->index == iterator->limit) {
        return -1 ;
    }

    name->groupName = PQgetvalue(iterator->result, iterator->index, 0) ;
    name->matName = PQgetvalue(iterator->result, iterator->index, 1) ;

    char *retStr = PQgetvalue(iterator->result, iterator->index, 2) ;
    char *endptr ;
    int nzs = strtol(retStr, &endptr, 10) ;
    if (*endptr != '\0' || *retStr == '\0' || errno == ERANGE || errno == EINVAL) {
        PQclear(iterator->result) ;
        free(iterator) ;
        return -1 ;
    }
    name->nzs = nzs ;

    iterator->index++ ;

    return 0 ;
}

void SCPA_PSQL_ResetIterator(SCPA_PSQL_TIMES_ITERATOR *iterator) {
    iterator->index = 0 ;
}

void SCPA_PSQL_DestroyIterator(SCPA_PSQL_TIMES_ITERATOR *iterator) {
    PQclear(iterator->result) ;
    free(iterator) ;
}

const char const *keys[6] = {
    "host",
    "port",
    "dbname",
    "user",
    "password",
    NULL
} ;

const char *values[6] ;

char initialized = 0 ;

void SCPA_PSQL_Clear() {
    if (connection) {
        PQfinish(connection) ;
        connection = NULL ;
    }
}

void SCPA_PSQL_Setup(char *host, char *port, char *user, char *db, char *pw) {

    if(connection) {
        SCPA_PSQL_Clear() ;
    }

    values[0] = host ;
    values[1] = port ;
    values[2] = db ;
    values[3] = user ;
    values[4] = pw ;
    values[5] = NULL ;

    initialized = 1 ;
}

static int SCPA_PSQL_Connect() {
    if (!initialized) return 1 ;

    if (connection) return 0 ;

    connection = PQconnectdbParams(
        keys,
        values,
        0
    ) ;

    if (PQstatus(connection) != CONNECTION_OK) {
        SCPA_PSQL_Clear() ;
        return 1 ;
    }

    return 0 ;
}

SCPA_PSQL_TIMES_ITERATOR *SCPA_PSQL_TimesForConfigCSR(char *group, char *name, char *kernel, char *loader) {

    SCPA_PSQL_TIMES_ITERATOR *iterator = malloc(sizeof(SCPA_PSQL_TIMES_ITERATOR)) ;
    if (!iterator) return NULL ;

    const char const *queryValues[4] = {
        name,
        group,
        kernel,
        loader
    } ;

    if (SCPA_PSQL_Connect()) {
        free(iterator) ;
        return NULL ;
    }

    PGresult *res = PQexecParams(
        connection,
        SELECT_TIMES_QUERY,
        SELECT_TIMES_PARAMS,
        selectOids,
        queryValues,
        NULL,
        NULL,
        0
    ) ;

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        PQclear(res) ;
        free(iterator) ;
        return NULL ;
    }

    iterator->index = 0 ;
    iterator->limit = PQntuples(res) ;
    iterator->result = res ;

    return iterator ;
}

SCPA_PSQL_TIMES_ITERATOR *SCPA_PSQL_Matrices() {

    SCPA_PSQL_TIMES_ITERATOR *iterator = malloc(sizeof(SCPA_PSQL_TIMES_ITERATOR)) ;
    if (!iterator) return NULL ;


    if (SCPA_PSQL_Connect()) {
        free(iterator) ;
        return NULL ;
    }

    PGresult *res = PQexecParams(
        connection,
        MATRICES_NAMES_QUERY,
        0,
        NULL,
        NULL,
        NULL,
        NULL,
        0
    ) ;

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        PQclear(res) ;
        free(iterator) ;
        return NULL ;
    }

    iterator->index = 0 ;
    iterator->limit = PQntuples(res) ;
    iterator->result = res ;

    return iterator ;
}