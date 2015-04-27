#!/bin/bash /home/philippe/bin.v2/qc

#pragma qc_import(../../lib/process)
#pragma qc_import(../../lib/unit_test)

#include <string>

using namespace std;

int main()
{
    Process f("grep secret");

    string ou = execapture(f);

    utAssertTrue( ou == "the secret is here\n" );
    
    return 0;
}
