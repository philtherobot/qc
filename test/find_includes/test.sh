#!/usr/bin/env bash

# Test the sed expression that finds includes in source files.

# This test extracts the expression from the script and makes
# tests with the expression.  It would have been a much slower
# test to generate a script for each kind of "#include" directives
# and run each scripts twice to see if qc picked up the dependency
# correctly.

which_qc="$(which qc)"
if [[ ${?} -ne 0 ]]
then
    error_and_exit unable to locate qc script
fi

expr="$(sed -n "s/    inclusions=(\$(sed -n '\(.*\)' \"\${d}\"))/\1/p" "${which_qc}")"
if [[ ${?} -ne 0 || -z "${expr}" ]]
then
    error_and_exit unable to extract sed expression from qc script
fi

output=$(sed -n "${expr}" <<EOF
#include "x.hpp"
# include "x.hpp"
 # include "x.hpp"
  #   include  "x.hpp"
#include "x space.hpp"
#include "sub directory/x space.hpp"
EOF
)

if [[ ${?} -ne 0 ]]
then
    error_and_exit non-zero exit status on sed during test
fi

expected="x.hpp
x.hpp
x.hpp
x.hpp
x space.hpp
sub directory/x space.hpp"

CHECK_OUTPUT "${output}" "${expected}"
