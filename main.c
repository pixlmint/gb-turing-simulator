#include <gb/gb.h>
#include <stdio.h>
#include "lib/turing_machine.h"
#include "lib/turing_logic.c"
#include "lib/turing_init.c"
#include "lib/gb_headers.h"
#include <gbdk/console.h>
#include <time.h>

// Preconfigured machine configurations
const char *preconfiguredMachines[] = {
    "0101000101001101001010010011000101000010100110001001010010011000010100001010011000010010000100100110000100010010001001111001",
    "010010100100110101000101001100010010100100110001010010100111010",
    "0100101001001101010001010011000100101001001100010100101001110100",
};

int main() {
    TuringMachine tm;
    initializeMachine(&tm);
    displayMenu(&tm);
    return 0;
}

void clearScreen() {
    for (int i = 0; i < 20 * 18; i++) {
        printf(" "); // Print spaces to clear the screen
    }
    gotoxy(0, 0); // Reset cursor position to top left
}

void displayMenu(TuringMachine *tm) {
    int programRunning = 1;
    while (programRunning) {
        uint8_t selectedMachine = 0; // Index of the selected machine
        uint8_t numMachines = sizeof(preconfiguredMachines) / sizeof(preconfiguredMachines[0]);
        uint8_t key;

        int keyPressed = 1;

        while (1) {
            if (keyPressed) {
                clearScreen();
                printf("Select a Machine:\n");

                // Display the list of machines
                for (uint8_t i = 0; i < numMachines; ++i) {
                    if (i == selectedMachine) {
                        printf("-> %u. Machine %u\n", i, i); // Highlight selected machine
                    } else {
                        printf("   %u. Machine %u\n", i, i);
                    }
                }
            }

            keyPressed = 1;
            key = joypad();

            if (key & J_UP) {
                selectedMachine = (selectedMachine == 0) ? numMachines - 1 : selectedMachine - 1;
            } else if (key & J_DOWN) {
                selectedMachine = (selectedMachine + 1) % numMachines;
            } else if (key & J_A) {
                // Use A button to select
                break;
            } else {
                keyPressed = 0;
            }
        }

        // Parse and run the selected machine
        parseConfiguration(tm, preconfiguredMachines[selectedMachine]);
        programRunning = runMachine(tm);
    }
}

void displayMachineState(TuringMachine *tm) {
    printf("\nCurrent State: %d\nTape\n:", tm->currentState);
    for (int i = 0; i < TAPE_LENGTH; i++) {
        const char tapeCharacter = tm->tape[i];
        if (i == tm->tapePosition) {
            printf("[%c", tapeCharacter);
        } else if (tapeCharacter != EMPTY_TAPE_VALUE) {
            printf("%c", tapeCharacter);
        }
    }
}

int runMachine(TuringMachine *tm) {
    int paused = 0;
    int selectPressed = 0;
    int lastExececution = 0;
    int machineTerminated = 0;

    while (selectPressed == 0 && machineTerminated == 0) {
        int now = time(NULL);
        if (now - lastExececution > 0) {
            machineTerminated = doMachineTurn(tm) == 1 ? 0 : 1;
            lastExececution = now;

            clearScreen();
            displayMachineState(tm);
        }
        // Check for pause
        if (joypad() & J_START) {
            paused = !paused; // Toggle pause state
            while (joypad() & J_START); // Wait for button release
        }

        if (paused) {
            delay(50);
            continue; // Skip the rest of the loop if paused
        }

        if (joypad() & J_SELECT) {
            selectPressed = 1; // Set flag to return to menu
            while (joypad() & J_SELECT); // Wait for button release
            continue; // Skip the rest of the loop
        }
    }

    clearScreen();
    if (selectPressed) {
        displayMenu(tm); // Return to the menu if Select was pressed
    } else {
        // After halting, display the final tape instead of going back to the menu
        if (tm->currentState == 1) {
            printf("The machine terminates");
        } else {
            printf("The machine does not terminate");
        }
        displayMachineState(tm);
    }

    while (1) {
        if (joypad() & J_SELECT) {
            while (joypad() & J_SELECT) {
            } // Wait for button release
            break;
        }
    }
    return 1;
}
