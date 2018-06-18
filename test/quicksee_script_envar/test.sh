#!/usr/bin/env bash

# QUICKSEE_SCRIPT environment variable should be
# set to the script's filepath.

cat >pgm.cpp <<EOF
#include <iostream>
int main()
{
    char * script = std::getenv("QUICKSEE_SCRIPT");
    if( !script )
    {
        return 1;
    }

    std::cout << script << '\n';
    return 0;
}
EOF

output="$(qc pgm.cpp)"
CHECK_EXIT_STATUS_OK
CHECK_OUTPUT "${output}" "pgm.cpp"

output="$(qc ./pgm.cpp)"
CHECK_EXIT_STATUS_OK
CHECK_OUTPUT "${output}" "./pgm.cpp"

abspath="$(readlink -f pgm.cpp)"
output="$(qc "${abspath}")"
CHECK_EXIT_STATUS_OK
CHECK_OUTPUT "${output}" "${abspath}"

cd ..
output="$(qc quicksee_script_envar/pgm.cpp)"
CHECK_EXIT_STATUS_OK
CHECK_OUTPUT "${output}" "quicksee_script_envar/pgm.cpp"
