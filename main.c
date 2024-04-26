#include <gb/gb.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lib/turing_machine.h"
#include "lib/turing_logic.c"
#include "lib/turing_init.c"
#include "lib/gb_headers.h"
#include <gbdk/console.h>
#include <time.h>
#include <string.h>
#include "lib/dyn_array.c"

// Preconfigured machine configurations
const char *preconfiguredMachines[] = {
    "Create Custom:",
    // "00:010100010100110100101001001100010100001010011000100101001001100001010000101001100001001000010010011000010001001000100",
    "04:010100010100110100101001001100010100001010011000100101001001100001010000101001100001001000010010011000010001001000100",
    "addition:010101010011010010100100110100010001000000010011000101000101001100010010001001001100010001000010001011000010010000010000101100000101000001010110000010010000010010110000010000000100000010000000101100000010000010000001000001011000000100000010000001000000101100000010100000001000000100110000001000100000001000000100110000000101000000010100110000000100100000001001001100000001000001000000010000010011000000010000001000000010000001001100000001000000010000000100000001001100000010010000000010000010110000000001001000000000101011000000000101000000010010011000000000100010000000100100110000000100001000010010110000101000000000010000101100000000001010000000000101011000000000010010000000000100101100000000001000000010000000000010000000101100000000000100000010000000000010000001011000000000001000001000000000001000001011000000000001010000000000001000001001100000000000100010000000000001000001001100000000000100100000000000010000001001100000000000010100000000000010100110000000000001001000000000000100100110000000000001000001000000000000100000100110000000000001000000100000000000010000001001100000000000010000000100000000000010000000100110000000000001000010000101011000010000000100000000000001000101100000000000001010000000000000101011000000000000010010000000000000100101100000000000001000000100000000000001001011000000000000010000010000000000000101011000000000000010001001000100",
};


uint8_t initialSize = sizeof(preconfiguredMachines) / sizeof(char *);
ConfigurationsArray *configurations;

void initializeConfigurations() {
    configurations = createArray(initialSize);

    for (uint8_t i = 0; i < initialSize; i++) {
        addConfiguration(configurations, preconfiguredMachines[i]);
    }
}

int main() {
    DISPLAY_ON;
    SHOW_BKG;

    initializeConfigurations();
    TuringMachine tm;
    initializeMachine(&tm);
    displayMenu(&tm);
    return 0;
}

void clearScreen() {
    cls();
}

char *my_strchr(const char *str, const uint8_t c) {
    while ((unsigned char) *str != '\0') {
        if ((unsigned char) *str == c)
            return (char *) str;
        str++;
    }
    if (c == '\0')
        return (char *) str;
    return NULL;
}

char *getMachineDescription(const char *machine) {
    static char description[MAX_CONFIG_LENGTH];
    uint8_t i = 0;
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


char *readConfiguration(char *message, uint8_t *confirmButton) {
    bool configEditing = 1;
    bool configChanged = 1;
    uint8_t configPosition = 0;
    char newConfig[MAX_CONFIG_LENGTH] = {0};
    for (uint8_t i = 0; i < MAX_CONFIG_LENGTH; i++) {
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
        const uint8_t key = joypad();

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
            while (joypad() & J_START) {
            }
            configEditing = 0;
            if (confirmButton != NULL) {
                *confirmButton = J_START;
            }
            continue;
        } else if (key & J_SELECT) {
            while (joypad() & J_SELECT) {
            }
            configEditing = 0;
            if (confirmButton != NULL) {
                *confirmButton = J_SELECT;
            }
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
    while (1) {
        uint8_t selectedMachine = 0; // Index of the selected machine
        const uint8_t numMachines = configurations->size;

        bool keyPressed = 1;

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
                    printf("%u. %s", i, getMachineDescription(configurations->configurations[i]));
                    printf("\n");
                }
            }

            keyPressed = 1;
            const uint8_t key = joypad();

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
            char *configForMemory = malloc(strlen("Custom:") + strlen(machineConfiguration) + 1);
            if (configForMemory) {
                strcpy(configForMemory, "Custom:");
                strcat(configForMemory, machineConfiguration);
                addConfiguration(configurations, configForMemory);
                free(configForMemory);
            } else {
                printf("Failed to add custom configuration");
                return;
            }
        } else {
            machineConfiguration = getMachineConfiguration(preconfiguredMachines[selectedMachine]);
        }
        uint8_t confirmButton;
        const char *machineInput = readConfiguration("Enter the machine input", &confirmButton);
        const char *separator = "111";
        char *completeConfig = malloc(strlen(machineInput) + strlen(machineConfiguration) + strlen(separator));
        strcpy(completeConfig, machineConfiguration);
        strcat(completeConfig, separator);
        strcat(completeConfig, machineInput);
        parseConfiguration(tm, completeConfig);
        free(completeConfig);

        runMachine(tm, confirmButton == J_SELECT ? EXECUTION_MODE_STEP : EXECUTION_MODE_CALCULATION);
    }
}

void displayMachineStateAtPosition(const TuringMachine *tm, const int8_t screenCenterPosition) {
    printf("\nCurrent State: %d\nTape:\n", tm->currentState);

    uint8_t printedCharacters = 0;
    int8_t currentPositionOnScreen = -1;
    for (uint8_t i = 0; i < DEVICE_SCREEN_WIDTH; i++) {
        const int16_t currentPosition = screenCenterPosition + i - DEVICE_SCREEN_WIDTH / 2;
        char tapeCharacter;
        if (currentPosition < 0 || currentPosition >= TAPE_LENGTH) {
            tapeCharacter = '_';
        } else {
            if (currentPosition == tm->tapePosition) {
                currentPositionOnScreen = printedCharacters;
            }
            tapeCharacter = tm->tape[currentPosition];
        }
        printf("%c", tapeCharacter);
        printedCharacters++;
    }
    if (currentPositionOnScreen != -1) {
        printf("\n");
        for (int i = currentPositionOnScreen - 1; i > 0; i--) {
            printf(" ");
        }
        printf("^");
    }
}

void displayMachineState(const TuringMachine *tm) {
    displayMachineStateAtPosition(tm, tm->tapePosition);
}

void runMachine(TuringMachine *tm, const uint8_t executionMode) {
    bool paused = 0;
    bool selectPressed = 0;
    uint8_t lastExececution = 0;
    bool machineTerminated = 0;
    uint16_t completedSteps = 0;

    while (!selectPressed && !machineTerminated && completedSteps < MAX_EXECUTIONS) {
        if (executionMode == EXECUTION_MODE_CALCULATION) {
            machineTerminated = doMachineTurn(tm) == 1 ? 0 : 1;
            completedSteps++;
        } else {
            const uint8_t now = time(NULL);
            if (now - lastExececution > 0) {
                machineTerminated = doMachineTurn(tm) == 1 ? 0 : 1;
                completedSteps++;
                lastExececution = now;

                clearScreen();
                printf("Running...");
                displayMachineState(tm);
            }
        }
        const uint8_t key = joypad();
        // Check for pause
        if (key & J_START) {
            paused = !paused; // Toggle pause state
            while (joypad() & J_START); // Wait for button release
        }

        if (paused) {
            delay(50);
            continue; // Skip the rest of the loop if paused
        }

        if (key & J_SELECT) {
            selectPressed = 1; // Set flag to return to menu
            while (joypad() & J_SELECT) {
            } // Wait for button release
        }
    }

    clearScreen();
    if (selectPressed) {
        displayMenu(tm); // Return to the menu if Select was pressed
        return;
    }

    // After halting, display the final tape instead of going back to the menu
    uint8_t virtualTapePosition = tm->tapePosition;
    bool keyPressed = 1;
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
        const uint8_t key = joypad();
        if (key & J_LEFT) {
            while (joypad() & J_LEFT) {
            }
            virtualTapePosition++;
            keyPressed = 1;
        }
        if (key & J_RIGHT) {
            while (joypad() & J_RIGHT) {
            }
            virtualTapePosition--;
            keyPressed = 1;
        }
        if (key & J_SELECT) {
            while (joypad() & J_SELECT) {
            } // Wait for button release
            break;
        }
    }
}
