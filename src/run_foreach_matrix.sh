#! /bin/bash

IFS=$'\n'

SCPA_PROGNAME=$1
shift

for i in $(cat matrices); do
    SCPA_MATRIX_ID=($i)
    ./ssh-exec.sh ${SCPA_PROGNAME} -- ${SCPA_PROGNAME} ${SCPA_MATRIX_ID[0]} ${SCPA_MATRIX_ID[1]} $@ | ./results_to_query.awk | psql -h localhost -p 5432 -U scpa -d scpa
done;