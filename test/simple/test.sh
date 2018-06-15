#!/usr/bin/env bash

# Simple script, everything works.

cat >pgm.cpp <<EOF
#include <iostream>
int main()
{
    std::cout << "Hello world!\n";
    return 0;
}
EOF

output="$(qc pgm.cpp)"
CHECK_EXIT_STATUS_OK
CHECK_OUTPUT "${output}" "Hello world!"
