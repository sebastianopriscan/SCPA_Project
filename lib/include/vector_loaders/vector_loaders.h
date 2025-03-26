#include "common/definitions.h"

/**
 * This method loads vectors from files in the following format
 *      line 0        : size         : int
 *      lines 1..size : vec[lineNum] : double
 */
MALLOCD double *SCPA_VECTOR_LoadFromFile(IN char *path) ;

MALLOCD double *SCPA_VECTOR_LoadRandom(IN int size, IN unsigned int seed) ;

MALLOCD double *SCPA_VECTOR_LoadRandomPattern(IN int size, IN unsigned int seed) ;