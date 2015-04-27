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
    
    string ou = execapture(pl);
    
    utAssertTrue( ou == "9\n" );

    return 0;
}

