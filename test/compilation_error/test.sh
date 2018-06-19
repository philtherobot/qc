#!/usr/bin/env bash

# Compilation and link error.

cat >compile.cpp <<EOF
invalid C++ text
EOF

output="$(qc compile.cpp 2>&1)"
CHECK_EXIT_STATUS 1

cat >link.cpp <<EOF
void func();
int main()
{
    func();  // func is missing
    return 0;
}
EOF

output="$(qc link.cpp 2>&1)"
CHECK_EXIT_STATUS 1
