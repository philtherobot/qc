#!/usr/bin/env bash

# Check that exit status is returned correctly.

cat >pgm.cpp <<EOF
int main()
{
    return 87;
}
EOF

qc pgm.cpp
CHECK_EXIT_STATUS 87
