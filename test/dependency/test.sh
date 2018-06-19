#!/usr/bin/env bash

# Check that a simple dependency on a header causes
# recompilation when the header changes.


#
# Setup a script and header dependency.
#

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
CHECK_OUTPUT "${output}" "Hello world!"


#
# Change dependency.
#

delay_for_fs

cat >dep.hpp <<EOF
char const * const message = "A different greeting.\n";
EOF

output="$(qc pgm.cpp)"
CHECK_EXIT_STATUS_OK
CHECK_OUTPUT "${output}" "A different greeting."


#
# Setup a two-levels deep dependency.
#

cat >string_counter.hpp <<EOF
#include <string>
#include <sstream>
#include "dep.hpp"
std::string counted()
{
	std::string ss(message);
	std::ostringstream os;
	os << ss.size() << ' ' << ss;
	return os.str();
}
EOF


cat >counted.cpp <<EOF
#include <iostream>
#include "string_counter.hpp"
int main()
{
	std::cout << counted() << '\n';
	return 0;
}
EOF

output="$(qc counted.cpp)"
CHECK_EXIT_STATUS_OK
CHECK_OUTPUT "${output}" "22 A different greeting."


#
# Change second-level dependency
#

delay_for_fs

cat >dep.hpp <<EOF
char const * const message = "Merry Halloween?\n";
EOF

output="$(qc counted.cpp)"
CHECK_EXIT_STATUS_OK
CHECK_OUTPUT "${output}" "17 Merry Halloween?"
