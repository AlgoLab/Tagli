=== Tagli

A program to determine splice junctions from NGS data.

Released under GPLv3 or later.

== Build

To build tagli you need gcc-4.7 or newer. The typical command to build
the project is:

    CXX=g++-4.7 make STATUS=production all

The available compilation flags are:
*  STATUS =  debug/production (default: debug)
*  CHECKS =  yes/no (default: yes if STATUS==debug else no) determined whether NDEBUG is enabled.
*  PROF =  yes/no (default: no). Determines whether profiling is enabled.
If you also wanto to run the tests, the target to compile is *test*.
