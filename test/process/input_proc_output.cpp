#!/bin/bash /home/philippe/bin.v2/qc

#pragma qc_import(../../lib/process)
#pragma qc_import(../../lib/unit_test)

#include <string>

using namespace std;

int main()
{
    Process f("grep secret");

    string in = "this is some input\n"
                "line 2\n"
                "the secret is here\n"
                "line 4\n";
                
    string ou = execapture(in, f);

    utAssertTrue( ou == "the secret is here\n" );
    
    return 0;
}

