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
    "",
    "0101000101001101001010010011000101000010100110001001010010011000010100001010011000010010000100100110000100010010001001111001",
    "010010100100110101000101001100010010100100110001010010100111010",
    "0100101001001101010001010011000100101001001100010100101001110100",
};

int main() {
    DISPLAY_ON;
    SHOW_BKG;
    TuringMachine tm;
    initializeMachine(&tm);
    displayMenu(&tm);
    return 0;
}

void clearScreen() {
    cls();
}

char *readConfiguration(char *message) {
    int configEditing = 1;
    int configChanged = 1;
    int configPosition = 0;
    char newConfig[MAX_CONFIG_LENGTH] = {0};
    for (int i = 0; i < MAX_CONFIG_LENGTH; i++) {
        newConfig[i] = '_';
    }
    while (configEditing) {
        if (configChanged) {
            clearScreen();
            printf("%s\n", message);
            for (int i = 0; i < MAX_CONFIG_LENGTH; i++) {
                if (newConfig[i] != '_') {
                    printf("%c", newConfig[i]);
                }
            }
        }

        configChanged = 1;
        int key = joypad();

        char addValue;

        if (key & J_A) {
            addValue = '1';
            while (joypad() & J_A); // Wait for button release
        } else if (key & J_B) {
            addValue = '0';
            while (joypad() & J_B); // Wait for button release
        } else if (key & J_LEFT) {
            while (joypad() & J_LEFT) {
            }
            if (configPosition > 0) {
                configPosition--;
                newConfig[configPosition] = '_';
            }
            continue;
        } else if (key & J_START) {
            configEditing = 0;
            continue;
        } else {
            configChanged = 0;
            continue;
        }

        newConfig[configPosition] = addValue;
        configPosition++;

        if (configPosition >= MAX_CONFIG_LENGTH) {
            break;
        }
    }

    return newConfig;
}

void displayMenu(TuringMachine *tm) {
    int programRunning = 1;
    while (programRunning) {
        int selectedMachine = 0; // Index of the selected machine
        int numMachines = sizeof(preconfiguredMachines) / sizeof(preconfiguredMachines[0]);
        int key;

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
                while (joypad() & J_UP); // Wait for button release
            } else if (key & J_DOWN) {
                selectedMachine = (selectedMachine + 1) % numMachines;
                while (joypad() & J_DOWN); // Wait for button release
            } else if (key & J_A) {
                // Use A button to select
                while (joypad() & J_A) {
                } // Wait for button release
                break;
            } else {
                keyPressed = 0;
            }
        }

        if (selectedMachine == 0) {
            char *customConfig = readConfiguration("Custom");
            parseConfiguration(tm, customConfig);
        } else {
            parseConfiguration(tm, preconfiguredMachines[selectedMachine]);
        }

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
            printf("Running...");
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
            while (joypad() & J_SELECT) {
            } // Wait for button release
        }
    }

    clearScreen();
    if (selectPressed) {
        displayMenu(tm); // Return to the menu if Select was pressed
    } else {
        // After halting, display the final tape instead of going back to the menu
        if (tm->currentState == 1) {
            printf("Accepted");
        } else {
            printf("Rejected");
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
