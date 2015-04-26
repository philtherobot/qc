#!/bin/bash /home/philippe/bin.v2/qc

#pragma qc_import(../../lib/process)

#include <string>

using namespace std;

int main()
{
    Process f("grep secret");
    
    string in = "this is some input\n"
                "line 2\n"
                "the secret is here\n"
                "line 4\n";

    execute(in, f);
     
    return 0;
}

