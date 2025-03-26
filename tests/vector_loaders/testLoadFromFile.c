#include <stdlib.h>
#include "vector_loaders/vector_loaders.h"

int main(void) {

    double *vector = SCPA_VECTOR_LoadFromFile("resources/testVector.vec") ;

    if (vector == NULL) return -1 ;

    int retVal = 0 ;

	if (vector[0] != 1.2233543) {
		retVal = -1 ;
		goto free_vector ;
	}
	if (vector[1] != 1.2344555e+4) {
		retVal = -1 ;
		goto free_vector ;
	}
	if (vector[2] != 0.34838) {
		retVal = -1 ;
		goto free_vector ;
	}
	if (vector[3] != 1.332e-10) {
		retVal = -1 ;
		goto free_vector ;
	}
	if (vector[4] != 2.330988) {
		retVal = -1 ;
		goto free_vector ;
	}

free_vector:
    free(vector) ;

    return retVal ;
}