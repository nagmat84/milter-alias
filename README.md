# `milter-alias`: A Sendmail-compatible Milter to Handle Sender Aliases

## Summary

## Building and Testing the Milter

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

Steps to test the milter (after the steps above)

 1. Change into the `build/tests`
 2. Run `ctest`

or

 1. Change into the `build`
 2. Run `tests/milter-alias-test`

## Runtime Configuration

tbd.
