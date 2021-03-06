#!/usr/bin/env bash

# That changing the script causes it to be recompiled.

cat >pgm.cpp <<EOF
#include <iostream>
int main()
{
    std::cout << "version 1\n";
    return 0;
}
EOF

output="$(qc pgm.cpp)"
CHECK_EXIT_STATUS_OK
CHECK_OUTPUT "${output}" "version 1"

delay_for_fs

cat >pgm.cpp <<EOF
#include <iostream>
int main()
{
    std::cout << "version 2\n";
    return 0;
}
EOF

output="$(qc pgm.cpp)"
CHECK_EXIT_STATUS_OK
CHECK_OUTPUT "${output}" "version 2"
