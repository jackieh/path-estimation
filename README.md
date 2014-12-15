Path Estimation
===============

This repository contains code for a C++ library which predicts a path for
two-dimensional movement, given a set of data points which contain a coordinate
pair and a timestamp.

### Dependencies

The library itself has the following dependencies.

- [Armadillo](http://arma.sourceforge.net/) for matrix operations

The test code which uses the library has the following dependencies.

- [PLplot](http://plplot.sourceforge.net/) for path visualization
- [NXJSON](https://bitbucket.org/yarosla/nxjson) for JSON parsing, already
  included in this repository

### Building and testing

The Makefile has a `test` target to compile a test program that uses the
library. The `run` target will run the test code on an example input.

### Documentation

The `docs` target in the Makefile will generate doxygen documentation.
