#!/usr/bin/env bash

# Source filename has spaces.

cat >"pgm with spaces.cpp" <<EOF
#include <iostream>
int main()
{
    std::cout << "Hello world!\n";
    return 0;
}
EOF

output="$(qc "pgm with spaces.cpp")"
CHECK_EXIT_STATUS_OK
CHECK_EQUAL "${output}" "Hello world!" "program output is wrong"
