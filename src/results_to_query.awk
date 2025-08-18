#! /bin/awk -f

BEGIN {
    query = "INSERT INTO results(group_name, matrix_name, loader, kernel_id, hll_size, time) VALUES "
    first = 1
}

{
    if (first) {
        query = query"('"$1"', '"$2"', '"$3"', '"$4"', "$5", "$6")"
        first = 0
    }
    else query = query", ('"$1"', '"$2"', '"$3"', '"$4"', "$5", "$6")"
    
}

END {
    print query";"
}
