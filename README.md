# GB Turing Simulator

A Universal Turing Machine Simulator written in C for GameBoy using the [GameBoy Dev Kit](https://github.com/gbdk-2020/gbdk-2020).

This library needs to be downloaded and unpacked inside the project directory (or change the `Makefile` and `CMakeLists.txt` references to it).

To run the tests, CUnit is required (I have it installed at `/usr/include`, if yours isn't there, you'll have to update your run configuration)
The tests can be run either by executing `make tests`, or alternatively from the `CMakeLists.txt` which I use in CLion.

The Rom can be built simply by running `make`.