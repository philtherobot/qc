# qc

Quick C is a shell script that compiles and runs a single-file C++ program.  The resulting binary is cached, so future invocations are fast.

Workflow:

1. Code with a text editor.
2. Save to `pgm.cpp` (or any other name).
3. On the shell: `qc pgm.cpp`.

This supports she-bang, so `./pgm.cpp` is possible too.
