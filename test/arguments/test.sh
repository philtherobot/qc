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
CHECK_EQUAL "${output}" "" "program output is wrong"

output="$(qc pgm.cpp arg1)"
CHECK_EXIT_STATUS_OK
expected="1-arg1-"
CHECK_EQUAL "${output}" "${expected}" "program output is wrong"

output="$(qc pgm.cpp arg1 arg2)"
CHECK_EXIT_STATUS_OK
expected=$'1-arg1-\n2-arg2-'
CHECK_EQUAL "${output}" "${expected}" "program output is wrong"

output="$(qc pgm.cpp arg1 "arg2 has spaces")"
CHECK_EXIT_STATUS_OK
expected=$'1-arg1-\n2-arg2 has spaces-'
CHECK_EQUAL "${output}" "${expected}" "program output is wrong"
