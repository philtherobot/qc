# Quicksee

Quicksee is a Bash shell script that compiles and runs a single-file C++ program.  The resulting binary is cached.  Quicksee performs dependency checking so future invocations are fast.

Workflow:

1. Code with a text editor.
2. Save your code to file.
3. In the shell: `qc pgm.cpp`.

This supports she-bang, so `./pgm.cpp` is possible too.

Command-line arguments are passed to the executable so your C++ script can receive them:

```
$ qc add.cpp 1 2
1 + 2 is 3

$
```


## What is it for?

Quicksee is *not* for industrial strength programming.  Quicksee is limited to small scripts: as soon as a script grows to medium size, you will need to use a classical build system and deployment.  Quicksee has limitations regarding heavy concurrent use and changing scripts.

Quicksee can be used to:

- Program in your favorite language more often.
- Quickly test an hypothesis.  If you do not need to actually execute the script, it might now be even quicker and more powerful to use [Compiler Explorer](https://godbolt.org/).
- Code up a one-use script that will be more comfortable to write in C++ versus the other choices (Bash, Python, etc).
- Get away from shell quoting, word splitting and quote removal issues.
- Write small utility scripts and scripts you use daily.  You may accumulate over time a small library of useful tools that make this easier.


## Requirements

- GNU C++
- Bash shell
- Unix environment (Linux or cygwin are examples)
- sed program
- hostname program


## Configuration

Quicksee finds its configuration (optional) in the `.quicksee` directory located in the user's home directory.  There, it reads a shell script file named `config.sh`.

The expectation from Quicksee of this script is that it will `export` the following environment variables:

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


## Options


### Force compilation `-f`

Quicksee will not perform an "up to date" check and will compile the script unconditionally prior to executing it normally.


### Update `-u`

Quicksee will ensure the cached executable is up to date with the sources like usual but will not launch the script.  This option is useful to ensure the next launch will be quick.


### Verbosity/debug `-v`

Quicksee will output a log of its execution.  It is not truly useful for users other than Quicksee's developers.  Still, it is available and it might help somebody troubleshoot some issue.


## Environment

Quicksee sets up an environment variable named `QUICKSEE_SCRIPT`.  This variable is accessible from the script when it executes.  `QUICKSEE_SCRIPT` contains the filepath to the script.


## How does it work?

Quicksee compiles the script using GNU g++.  The intermediary file (.o) and  executable file are stored in a cache located in the `.quicksee/cache` directory in the user's home directory.  The cache is keyed by the hostname and the absolute filepath of the source file.  Because the hostname is part of the cache key, the cache will work correctly if your home directory is shared across hosts.

Quicksee actually performs a dependency test on the `#include` directives found in the source files.  Only the files included *with double quotes* are considered for dependency checking.  Files included with less-than and greater-than are considered to be "system" headers that do not change.

The "is up to date" check is performed by comparing file modification times.


## Limitations and working around them

Quicksee can only build scripts made of a single *compilation unit*: one source file as input.  What if you have a library of functions you want to share across many scripts?  You will have to place those functions, with their definitions, in a separate file and use an `#include` directive to bring in the functions in each of your scripts.

Quicksee links to *all* the libraries listed in the configuration whether your script needs them or not.  Quicksee relies on the linker to drop unused identifiers.

Your script will receive the arguments the user gave on the command-line when the script was invoked.  But the very first one, argument zero, which is usually the script's filename, will not be correct.  Argument zero will be the cached executable filename.  A workaround is to read the `QUICKSEE_SCRIPT` environment variable that has been setup for the executing program.  The variable contains the filepath of the script given to the `qc` command.

Quicksee uses the file modification times to verify if the script needs to be recompiled.  Because file times have limited precision (which depends on a variety of factors), if a compilation-modification-dependency check cycle is done very quickly, the dependency check could fail and declare that the file was not changed.  This can occur in scenarios of scripts being launch by another script very quickly.  For example, if a main script generates a sub script based on some input and launches it, the source file will be compiled and its cached executable will have a file modification time equal to the time of compilation.  In this scenario, the main script, upon return of the sub script, rewrites the sub script to something different.  If the main script accomplished this quickly enough that the new sub script has the same file modification time as the cached executable, then Quicksee's dependency will fail by declaring that the cache is up to date.  To workaround this:
- Make the main script wait for at least your file system's time precision.
- Change the source file's name at every generation, or every X generation (like "f1" to "f10" then cycle back).
- Since the main script knows it changed the source file, it could launch it with the "force compilation" option (-f).

Quicksee cannot reliably handle multiple invocations of the same script at the same time that would result in a compilation.  The mutiple executing `qc` invocations will all try to compile the cached executable and write to the same output file at the same time which will cause a compilation failure.  Possible workaround are:
- Do nothing: maybe you can handle/ignore a failure like this from time to time.  
- Use the update option (-u) to ensure the cached executable is up to date before launching the multiprocessing system.
- Future: the cache's implementation may become safe of multiple writers and make this problem completely go away.


## Future

### Important

- Find or write a small companion library that makes it easy to do things that are already easy in shell scripting: launching subprocesses, piping to/from them, manipulating environment variables, etc.

### Wishes

- Detect invalid options yet support scripts starting with dash.
- Add an option to compile the executable and place it in the current directory.
- Automatically prune the cache to control its size.  Maybe options for the user to control its maximum size?  Maybe simply a manual pruning of the cache the user has to launch?
- Make the configuration format YAML or some such rather than a shell script.  It could be very constraining that the configuration is a shell script if the implementation of Quicksee were to change from Bash to say, C++.
- Try to spoof argument zero.  Or offer a library that the script can use to recover the script's filename.
- Make the cache safe for multiple writers.
