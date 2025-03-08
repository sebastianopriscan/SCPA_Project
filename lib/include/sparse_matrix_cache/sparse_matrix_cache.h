#ifndef SPARSE_MATRIX_CACHE_H
#define SPARSE_MATRIX_CACHE_H

#include <stdio.h>
#include "common/definitions.h"

int SCPA_SPCACHE_OpenMatrix(IN char *matrixName, IN char *matrixGroup, IN char *cache_root_dir, OUT FILE **file) ;

#endif