#!/bin/bash /home/philippe/bin.v2/qc

#pragma qc_import(../../lib/process)

int main()
{
    Process f("grep secret");

    execute(f);
    
    return 0;
}

