#!/bin/bash /home/philippe/bin.v2/qc

#pragma qc_import(../../lib/process)

#include <string>
#include <iostream>

using namespace std;

void exec()
{
    exec("cat demo.cpp")();
}

void capture_stdout()
{
    string o;
    exec("cat demo.cpp").ostring(&o)();
     
    cout << "'" << o << "'\n";
}

void return_capture()
{
    cout << "'" << exec("cat demo.cpp").ostring()() << "'\n";
}

void feed_stdin()
{
   string in = "line 1\n"
                "line 2\n"
                "the secret is here\n"
                "line 4\n"
                "and also a secret here\n"
                "line 6\n";

    exec("grep secret").istring(in)();
}

void use_stdin()
{
    exec("grep secret")();
}

void pipe()
{
    exec("cat demo.cpp").pipe("grep secret")();
}

int main()
{
    //exec();
    //capture_stdout();
    return_capture();
    //feed_stdin();
    //use_stdin();
    //pipe();

    return 0;
}

