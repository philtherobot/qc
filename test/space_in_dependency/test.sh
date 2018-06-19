#!/usr/bin/env bash

# Check that dependency checking can handle 
# included filepaths with spaces.

if [[ ! -e "dir with spaces" ]]
then
    mkdir "dir with spaces"
fi

cat >"dir with spaces/utility functions.cpp" <<EOF
int add(int a, int b)
{
    return a + b;
}
EOF

cat >pgm.cpp <<EOF
#include <iostream>
#include "dir with spaces/utility functions.cpp"
int main()
{
	int a=3;
	int b=4;
    std::cout << a << " + " << b << " = " << add(a,b) << '\n';
    return 0;
}
EOF

output="$(qc pgm.cpp)"
CHECK_EXIT_STATUS_OK
CHECK_OUTPUT "${output}" "3 + 4 = 7"
