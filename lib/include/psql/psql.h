#ifndef SCPA_PSQL_H
#define SCPA_PSQL_H

#include "common/definitions.h"

/**
 * Opaque struct definition for the query iterators
 */
typedef struct _SCPA_PSQL_TIMES_ITERATOR SCPA_PSQL_TIMES_ITERATOR ;

/**
 * Structure for containing matrix name and group
 */
typedef struct _SCPA_MatrixName {
    char *groupName ;
    char *matName ;
    int nzs ;
} SCPA_MatrixName ;

/**
 * Setup PSQL Data for connections.
 * This function should be called before any other routine invocation
 * 
 * @param host : The DBMS host
 * @param port : The DBMS port
 * @param user : The DBMS username
 * @param db : The DBMS database
 * @param pw : The DBMS user's password
 */
void SCPA_PSQL_Setup(IN char *host, IN char *port, IN char *user, IN char *db, IN char *pw) ;

/**
 * Clear PSQL Data.
 * This function should be called before exiting the program or when the library is not needed
 */
void SCPA_PSQL_Clear() ;

/**
 * Get the next time from a query result.
 * @param iterator : a SCPA_PSQL_TIMES_ITERATOR handle
 * @returns a number greater than 0 representing the time in nanoseconds the kernel spent multiplying,
 *          -1 in case there are errors or no more times to return.
 */
long SCPA_PSQL_NextTime(SCPA_PSQL_TIMES_ITERATOR *iterator) ;

/**
 * Get the next matrix stored in the schema.
 * @param iterator : a SCPA_PSQL_TIMES_ITERATOR handle
 * @returns 0 in case there is more data to be returned, in which case the data is stored in the name param,
 *          -1 in case there are errors or no more times to return.
 */
int SCPA_PSQL_NextMatrix(SCPA_PSQL_TIMES_ITERATOR *iterator, SCPA_MatrixName *name) ;

/**
 * Reset an iterator
 * @param iterator : the iterator to reset
 */
void SCPA_PSQL_ResetIterator(SCPA_PSQL_TIMES_ITERATOR *iterator) ;

/**
 * Free an iterator in case reading the remaining data is not necessary
 * @param iterator : a SCPA_PSQL_TIMES_ITERATOR handle
 */
void SCPA_PSQL_DestroyIterator(SCPA_PSQL_TIMES_ITERATOR *iterator) ;

/**
 * Get the Recorded times for a specific matrix-kernel-loader combination
 * @param group : the matrix's group
 * @param name : the matrix's name
 * @param kernel : the matrix multiplying kernel
 * @param loader : the CSR loader for the matrix
 * @returns a SCPA_PSQL_TIMES_ITERATOR for retrieving the times
 */
SCPA_PSQL_TIMES_ITERATOR *SCPA_PSQL_TimesForConfigCSR(char *group, char *name, char *kernel, char *loader) ;

/**
 * Get the Recorded times for a specific matrix-kernel-loader combination
 * @returns a SCPA_PSQL_TIMES_ITERATOR for retrieving the names and groups of the matrices
 */
SCPA_PSQL_TIMES_ITERATOR *SCPA_PSQL_Matrices() ;

#endif