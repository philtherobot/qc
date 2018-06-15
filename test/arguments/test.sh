#!/usr/bin/env bash

# Check that command-line arguments are passed correctly.

cat >pgm.cpp <<EOF
#include <iostream>
int main(int argc, char ** argv)
{
    for(int i=1; i < argc; ++i)
    {
        std::cout << i << '-' << argv[i] << "-\n";
    }
    return 0;
}
EOF

output="$(qc pgm.cpp)"
CHECK_EXIT_STATUS_OK
CHECK_OUTPUT "${output}" ""

output="$(qc pgm.cpp arg1)"
CHECK_EXIT_STATUS_OK
expected="1-arg1-"
CHECK_OUTPUT "${output}" "${expected}"

output="$(qc pgm.cpp arg1 arg2)"
CHECK_EXIT_STATUS_OK
expected=$'1-arg1-\n2-arg2-'
CHECK_OUTPUT "${output}" "${expected}"

output="$(qc pgm.cpp arg1 "arg2 has spaces")"
CHECK_EXIT_STATUS_OK
expected=$'1-arg1-\n2-arg2 has spaces-'
CHECK_OUTPUT "${output}" "${expected}"
