#!/usr/bin/env bash

# Testing the "force compilation" option.

cat >pgm.cpp <<EOF
#include <iostream>
int main()
{
    std::cout << __TIME__ << "\n";
    return 0;
}
EOF

initial_output="$(qc pgm.cpp)"

second_output="$(qc pgm.cpp)"

if [[ "${initial_output}" != "${second_output}" ]]
then
    check_failed script should not have recompiled
fi

forced_output="$(qc -f pgm.cpp)"

if [[ "${initial_output}" == "${forced_output}" ]]
then
    check_failed script was not recompiled
fi
