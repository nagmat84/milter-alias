# `milter-alias`: A Sendmail-compatible Milter to Handle Sender Aliases

## Summary

## Building the Milter

The following tools are required

 - A C11-compatible compiler, e.g. gcc
 - CMake version 3.27 or higher
 - Ninja

The following additional libraries are needed
 - `libmilter`
 - OpenLDAP
 - Systemd

Steps to build the milter

 1. Change into the `build` directory
 2. Run `cmake -G Ninja ..`
 3. Run `ninja`

## Testing the Milter

The following tools are _additional_ required:

 - [Check library](https://libcheck.github.io/check/) for C unit tests
 - Gcov for C coverage

The following additional libraries are needed

 - `libcheck`
 - `libgcov`

Steps to test the milter

 1. Change into the `build` directory
 2. Run `cmake -G Ninja ..`
 3. Run `ninja`
 4. Run `tests/milter-alias-test`

Get coverage analysis (after the steps above)

 1. Run `gcov tests/CMakeFiles/milter-alias-test.dir/__/src/*.c.o`

This creates a text file `<something>.c.gcov` inside the `build` directory for
each tested C source file.
This text file is an annotated source file with a counter how often each
line of code has been hit.
Lines starting with `#####:` are the most interesting ones as these are
lines of code which have not been executed at all.

## Runtime Configuration

tbd.
