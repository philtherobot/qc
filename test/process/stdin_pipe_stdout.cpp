#!/bin/bash /home/philippe/bin.v2/qc

#pragma qc_import(../../lib/process)

int main()
{
    ProcessList pl;
    pl.push_back(Process("grep secret"));
    pl.push_back(Process("wc -w"));
    
    execute(pl);
    
    return 0;
}

