#!/usr/bin/env bash

# Update option.

cat >pgm.cpp <<EOF
#include <iostream>
int main()
{
    std::cout << "Hello world!\n";
    return 0;
}
EOF

output="$(qc -u pgm.cpp)"
CHECK_EXIT_STATUS_OK
CHECK_OUTPUT "${output}" ""

output="$(qc pgm.cpp)"
CHECK_EXIT_STATUS_OK
CHECK_OUTPUT "${output}" "Hello world!"
