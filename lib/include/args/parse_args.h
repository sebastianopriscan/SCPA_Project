#ifndef SCPA_PARSE_ARGS_H
#define SCPA_PARSE_ARGS_H

#include <stdio.h>
#include "common/definitions.h"

typedef enum _SCPA_LoaderClass {
    SCPA_DIRECT_CSR_CLASS,
    SCPA_DIRECT_HLL_CLASS
} SCPA_LoaderClass ;

const char *SCPA_LoaderClassToString(IN SCPA_LoaderClass) ;

typedef struct _SCPA_ParsedArgs {
    void *loader ;
    SCPA_LoaderClass loaderClass ;
    double *output ;
    int times ;
} SCPA_ParsedArgs ;

MALLOCD SCPA_ParsedArgs *SCPA_ParseArgs(IN int argc, IN char **argv) ;

#endif
