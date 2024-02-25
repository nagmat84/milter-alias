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

## Runtime Configuration

tbd.
