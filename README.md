# Quicksee

Quicksee is a Bash shell script that compiles and runs a single-file C++ program.  The resulting binary is cached.  Quicksee performs dependency checking so future invocations are fast.

Workflow:

1. Code with a text editor.
2. Save your code to file.
3. In the shell: `qc pgm.cpp`.

This supports she-bang, so `./pgm.cpp` is possible too.

Command-line arguments are passed to the executable so your program can receive them:

```
$ qc add.cpp 1 2
1 + 2 is 3

$
```


## What is it for?

Quicksee is *not* for industrial strength programming.  Quicksee is limited to small programs: as soon as a program grows to medium size, you will need to use a classical build system and deployment.

Quicksee can be used to:

- Program in your favorite language more often.
- Quickly test an hypothesis.  But it might now be even quicker and more powerful to use [Compiler Explorer](https://godbolt.org/), if you do not need to actually execute the program.
- Code up a one-use program that will be more comfortable to write in C++ versus the other choices (Bash, Python, etc).
- Get away from shell quoting, word splitting and quote removal issues.
- Write small utility programs and scripts you use daily.  You might accumulate a small library of useful tools that make this easier.


## Requirements

- GNU C++
- Bash shell
- Unix environment (Linux or cygwin are examples)


## Configuration

Quicksee finds its configuration (if any) in the `.quicksee` directory located in the user's home directory.  There, it reads a shell script file named `config.sh`.

The expectation from Quicksee of this script is that it will export the following environment variables:

- DEFINES: compiler options to set macros
- INCPATHS: compiler options to add include paths
- LIBS: linker/compiler options to include libraries
- OPTS: compiler options needed in general

For example:

```
export DEFINES="-DSTATIC_LIB -DNDEBUG"
export INCPATHS="-I/usr/include/qt5"
export LIBS="-lboost_filesystem"
export OPTS="-Wall -std=gnu++11"
```


## Limitations and working around them

Quicksee can only build programs made of a single *compilation unit*: one source file as input.  What if you have a library of functions you want to share across many programs?  You will have to place those functions, with their definitions, in a separate file and use an `#include` directive to bring in the functions in each of your programs.

Quicksee links to *all* the libraries listed in the configuration.  Quicksee relies on the linker to drop unused identifiers.

Your script will receive the arguments the users gave on the command-line when the script was invoked.  But the very first one, argument zero, which is usually the script's filename, will not be correct.  Argument zero will be the cached executable filename.  There is no workaround at this time.


## How does it work?

Quicksee compiles the program using GNU g++.  The intermediary file (.o) and  executable file are stored in a cache located in the `.quicksee/cache` directory in the user's home directory.  The cache is keyed by the hostname and the absolute filepath of the source file.

Quicksee actually performs a dependency test on the `#include` directives found in the source files.  Only the files included *with double quotes* are considered for dependency checking.  Files included with less-than and greater-that are considered to be "system" headers that do not change.


## Options

Quicksee has a single option: "-v" for verbose/debug execution.  


## Future

- Add an option to compile the executable and place it in the current directory.
- Automatically prune the cache to control its size.  Maybe options for the user to control its maximum size?
- Manual pruning of the cache.
- Option to force recompilation.
- Find or write a small companion library that makes it easy to do things that are already easy in shell scripting: launching subprocesses, piping to/from them, manipulating environment variables, etc.
- Make the configuration format YAML or some such rather than a shell script.  It could be very constraining that the configuration is a shell script if the implementation of Quicksee were to change from Bash to say, C++.
- Add tests.
- Try to spoof argument zero.  Or offer a library that the program can use to recover the script's filename.  Or place the filename in an environment variable such as `QUICKSEE_SCRIPT`.
