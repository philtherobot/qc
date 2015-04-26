#!/bin/bash /home/philippe/bin.v2/qc

#pragma qc_import(../../lib/process)
#pragma qc_import(../../lib/unit_test)

#include <string>

using namespace  std;

int main()
{
    ProcessList pl;
    pl.push_back(Process("grep secret"));
    pl.push_back(Process("wc -w"));
    
    string in = "line 1\n"
                "line 2\n"
                "the secret is here\n"
                "line 4\n"
                "and also a secret here\n"
                "line 6\n";
    
    string ou = execapture(in, pl);
    
    utAssertTrue( ou == "9\n" );

    return 0;
}

