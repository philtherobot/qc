#!/usr/bin/env bash

# The invoked script is in a directory with spaces.

if [[ ! -e "dir with spaces" ]]
then
    mkdir "dir with spaces"
fi

cat >"dir with spaces/pgm.cpp" <<EOF
#include <iostream>
int main()
{
    std::cout << "Hello world!\n";
    return 0;
}
EOF

output="$(qc "dir with spaces/pgm.cpp")"
CHECK_EXIT_STATUS_OK
CHECK_OUTPUT "${output}" "Hello world!"
