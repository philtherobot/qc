#!/usr/bin/env bash

# Check that a simple dependency on a header causes
# recompilation when the header changes.

cat >pgm.cpp <<EOF
#include <iostream>
#include "dep.hpp"
int main()
{
    std::cout << message;
    return 0;
}
EOF

cat >dep.hpp <<EOF
char const * const message = "Hello world!\n";
EOF

output="$(qc pgm.cpp)"
CHECK_EXIT_STATUS_OK
CHECK_EQUAL "${output}" "Hello world!" "program output is wrong"

delay_for_fs

cat >dep.hpp <<EOF
char const * const message = "A different greeting.\n";
EOF

output="$(qc pgm.cpp)"
CHECK_EXIT_STATUS_OK
CHECK_EQUAL "${output}" "A different greeting." "program output is wrong"
