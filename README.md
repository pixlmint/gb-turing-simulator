# GB Turing Simulator

A Universal Turing Machine Simulator written in C for GameBoy using the [GameBoy Dev Kit](https://github.com/gbdk-2020/gbdk-2020).

This library needs to be downloaded and unpacked inside the project directory (or change the `Makefile` and `CMakeLists.txt` references to it).

To run the tests, CUnit is required (I have it installed at `/usr/include`, if yours isn't there, you'll have to update your run configuration)
The tests can be run either by executing `make tests`, or alternatively from the `CMakeLists.txt` which I use in CLion.

The Rom can be built simply by running `make`.

## How to use it
The generated ROM File can be used with any device able to emulate GB/GBC/GBA consoles, or alternatively
[this](https://taisel.github.io/GameBoy-Online/) online emulator can be used.

### Screens
#### Main Menu
![main_menu](/assets/main_menu.png)

Inputs: 
- UP: move cursor up in list
- DOWN: move cursor down in list
- A: currently marked machine

The main menu screen, where a machine can be selected. Or, if selecting "Create Custom", you'll be able to enter your own configuration.

#### Machine Input & Machine Configuration Menu
![input](/assets/machine_input.png)

Inputs:
- A: Enter "1"
- B: Enter "0"
- UP: Whitespace
- START: Start the machine

In this menu you'll be able to enter your machine configuration (if "Create Custom" was selected), or the
machine input (initial Tape Contents) can be defined

#### Machine Running
![machine running](/assets/machine_running.png)

Inputs:
- START: Pause
- SELECT: Cancel, return to main menu

#### Machine Terminated
![machine terminated](/assets/machine_done.png)

Inputs:
- LEFT: Move tape left
- RIGHT: Move tape right
- SELECT: Return to main menu

Shows the final result of the calculation, the number of steps required, and the final tape with the current tape head shown