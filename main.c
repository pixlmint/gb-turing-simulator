#include <gb/gb.h>
#include <stdio.h>
#include <stdlib.h>
#include "lib/turing_machine.h"
#include "lib/turing_logic.c"
#include "lib/turing_init.c"
#include "lib/gb_headers.h"
#include <gbdk/console.h>
#include <time.h>
#include <string.h>

// Preconfigured machine configurations
const char *preconfiguredMachines[] = {
    "Custom Machine:",
    "00:010100010100110100101001001100010100001010011000100101001001100001010000101001100001001000010010011000010001001000100",
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

char *my_strchr(const char *str, int c) {
    while (*str != '\0') {
        if (*str == c)
            return (char *) str;
        str++;
    }
    if (c == '\0')
        return (char *) str;
    return NULL;
}

char *getMachineDescription(const char *machine) {
    static char description[MAX_CONFIG_LENGTH];
    int i = 0;
    while (machine[i] != ':' && machine[i] != '\0') {
        description[i] = machine[i];
        i++;
    }
    description[i] = '\0'; // Null terminate the string
    return description;
}

char *getMachineConfiguration(const char *machine) {
    static char configuration[MAX_CONFIG_LENGTH];
    const char *startOfConfiguration = my_strchr(machine, ':');
    if (startOfConfiguration != NULL) {
        startOfConfiguration++; // Skip the colon
        strcpy(configuration, startOfConfiguration);
    } else {
        configuration[0] = '\0'; // If no colon is found, return an empty string
    }
    return configuration;
}


char *readConfiguration(char *message, int *confirmButton) {
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
            while(joypad() & J_START) {}
            configEditing = 0;
            *confirmButton = J_START;
            continue;
        } else if (key & J_SELECT) {
            while(joypad() & J_SELECT) {}
            configEditing = 0;
            *confirmButton = J_SELECT;
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
                        printf("-> "); // Highlight selected machine
                    } else {
                        printf("   ");
                    }
                    printf("%u. %s", i, getMachineDescription(preconfiguredMachines[i]));
                    printf("\n");
                }
            }

            keyPressed = 1;
            key = joypad();

            if (key & J_UP) {
                selectedMachine = selectedMachine == 0 ? numMachines - 1 : selectedMachine - 1;
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

        char *machineConfiguration;
        if (selectedMachine == 0) {
            machineConfiguration = readConfiguration("Custom", 0);
            // char *configForMemory = strcat("Custom:", machineConfiguration);
            // addNewConfiguration(configForMemory);
        } else {
            machineConfiguration = getMachineConfiguration(preconfiguredMachines[selectedMachine]);
        }
        int confirmButton;
        char *machineInput = readConfiguration("Enter the machine input", &confirmButton);
        const char *separator = "111";
        char *completeConfig = malloc(sizeof(&machineInput) + sizeof(&machineConfiguration) + sizeof(&separator));
        completeConfig[0] = '\0';
        strcat(completeConfig, machineConfiguration);
        strcat(completeConfig, separator);
        strcat(completeConfig, machineInput);
        parseConfiguration(tm, completeConfig);

        programRunning = runMachine(tm, confirmButton == J_SELECT ? EXECUTION_MODE_STEP : EXECUTION_MODE_CALCULATION);
    }
}

void displayMachineStateAtPosition(TuringMachine *tm, int screenCenterPosition) {
    printf("\nCurrent State: %d\nTape:\n", tm->currentState);

    for (int i = 0; i < DEVICE_SCREEN_WIDTH; i++) {
        const int currentPosition = screenCenterPosition + i - DEVICE_SCREEN_WIDTH / 2;
        if (currentPosition < 0 || currentPosition >= TAPE_LENGTH) {
            continue;
        }
        char tapeCharacter = tm->tape[currentPosition];
        printf("%c", tapeCharacter);
    }
}

void displayMachineState(TuringMachine *tm) {
    displayMachineStateAtPosition(tm, tm->tapePosition);
}

int runMachine(TuringMachine *tm, int executionMode) {
    int paused = 0;
    int selectPressed = 0;
    int lastExececution = 0;
    int machineTerminated = 0;
    int completedSteps = 0;

    while (selectPressed == 0 && machineTerminated == 0 && completedSteps < MAX_EXECUTIONS) {
        if (executionMode == EXECUTION_MODE_CALCULATION) {
            machineTerminated = doMachineTurn(tm) == 1 ? 0 : 1;
            completedSteps++;
        } else {
            int now = time(NULL);
            if (now - lastExececution > 0) {
                machineTerminated = doMachineTurn(tm) == 1 ? 0 : 1;
                completedSteps++;
                lastExececution = now;

                clearScreen();
                printf("Running...");
                displayMachineState(tm);
            }
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
        return 1;
    }

    // After halting, display the final tape instead of going back to the menu
    int virtualTapePosition = tm->tapePosition;
    int keyPressed = 1;
    while (1) {
        if (keyPressed) {
            clearScreen();
            if (tm->currentState == 1) {
                printf("Accepted");
            } else {
                printf("Rejected");
            }
            printf("\nNum Calculations: %d", completedSteps);
            displayMachineStateAtPosition(tm, virtualTapePosition);
        }
        keyPressed = 0;
        if (joypad() & J_LEFT) {
            while (joypad() & J_LEFT) {
            }
            virtualTapePosition++;
            keyPressed = 1;
        }
        if (joypad() & J_RIGHT) {
            while (joypad() & J_RIGHT) {
            }
            virtualTapePosition--;
            keyPressed = 1;
        }
        if (joypad() & J_SELECT) {
            while (joypad() & J_SELECT) {
            } // Wait for button release
            break;
        }
    }
    return 1;
}
