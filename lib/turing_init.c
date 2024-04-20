#include <stdio.h>
#include "turing_machine.h"

int countZerosBeforeNextOne(const char **str) {
    int count = 0;
    while (**str != '\0' && **str != '1') {
        if (**str == '0') {
            count++;
        }
        (*str)++;
    }
    return count;
}

void parseBinaryValue(char **config, int *value) {
    *value = countZerosBeforeNextOne(config);
}

int parseBinaryValueAndSkipSeparator(char **config, int *value) {
    parseBinaryValue(config, value);
    if (**config == '1') {
        (*config)++; // Skip the '1' separator
        return 1;
    }

    return 0;
}

char getSymbolFromValue(int value) {
    if (value == 0) {
        return '0';
    }
    if (value == 1) {
        return '1';
    }
    if (value == 2) {
        return '_';
    }
    if (value < 29) {
        return value + 94;
    }
    return '_';
}

char parseDirection(const char **config) {
    const int count = countZerosBeforeNextOne(config);
    if (count == 1) {
        return 'L'; // Left
    }
    if (count == 2) {
        return 'R'; // Right
    }
    return '\0'; // Handle unexpected input
}

void initializeMachine(TuringMachine *tm) {
    // Fill the tape with the blank symbol, which is '-'
    for (int i = 0; i < TAPE_LENGTH; i++) {
        tm->tape[i] = EMPTY_TAPE_VALUE;
    }

    // Start in the middle of the tape or at the position you prefer
    tm->tapePosition = TAPE_LENGTH / 2;

    // Set the machine to the start state, which is state 0 (q1)
    tm->currentState = 0;
}

void writeTape(const char **config, TuringMachine *tm) {
    int tapePosition = tm->tapePosition;
    while (**config != '\0') {
        tm->tape[tapePosition] = **config;
        (*config)++;
        tapePosition++;
    }
}

void clearMachine(TuringMachine *tm) {
    const int statesCount = sizeof(tm->states) / sizeof(tm->states[0]);
    for (int i = 0; i < statesCount; i++) {
        tm->states[i].isUsed = 0;
        const int transitionsCount = sizeof(tm->states[i].transitions) / sizeof(tm->states[i].transitions[0]);
        for (int x = 0; x < transitionsCount; x++) {
            tm->states[i].transitions[x].isUsed = 0;
        }
    }
}

int getNextAvailableTransitionIndex(State *state) {
    for (int i = 0; i < MAX_TRANSITIONS; i++) {
        if (state->transitions[i].isUsed == 0) {
            return i;
        }
    }
    return -1;
}

void parseConfiguration(TuringMachine *tm, const char *config) {
    int stateIndex = 0, inputReached = 0;

    clearMachine(tm);

    if (*config == '1') {
        config++;
    }

    // Parse each transition in the state
    while (*config != '\0' && inputReached == 0) {
        // Parse current state
        parseBinaryValueAndSkipSeparator(&config, &stateIndex);
        stateIndex = stateIndex - 1;

        // Parse read symbol
        int readSymbolValue;
        parseBinaryValueAndSkipSeparator(&config, &readSymbolValue);
        char readSymbol = getSymbolFromValue(readSymbolValue - 1);

        // Parse next state
        int nextState;
        parseBinaryValueAndSkipSeparator(&config, &nextState);
        nextState = nextState - 1;

        // Parse write symbol
        int writeSymbolValue;
        parseBinaryValueAndSkipSeparator(&config, &writeSymbolValue);
        char writeSymbol = getSymbolFromValue(writeSymbolValue - 1);

        // get the next available transition index
        int transitionIndex = getNextAvailableTransitionIndex(&tm->states[stateIndex]);

        // Parse direction
        const char moveDirection = parseDirection(&config);

        // Assign parsed values to the current transition
        tm->states[stateIndex].transitions[transitionIndex].nextState = nextState;
        tm->states[stateIndex].transitions[transitionIndex].readSymbol = readSymbol;
        tm->states[stateIndex].transitions[transitionIndex].writeSymbol = writeSymbol;
        tm->states[stateIndex].transitions[transitionIndex].moveDirection = moveDirection;
        tm->states[stateIndex].transitions[transitionIndex].isUsed = 1;

        tm->states[stateIndex].isUsed = 1;
        tm->states[nextState].isUsed = 1;

        // Skip the '11' that separates transitions)
        if (*config == '1') {
            config += 1;
        }
        if (*config == '1') {
            config += 1;
        }
        if (*config == '1') {
            inputReached = 1;
            config += 1;
        }
    }

    // Initialize rest of the machine
    initializeMachine(tm);

    writeTape(&config, tm);

    tm->numTransitions = 0;
    tm->numStates = 0;
    for (int i = 0; i < MAX_STATES; i++) {
        if (tm->states[i].isUsed == 1) {
            tm->numStates += 1;
        }
        for (int x = 0; x < MAX_TRANSITIONS; x++) {
            if (tm->states[i].transitions[x].isUsed == 1) {
                tm->numTransitions += 1;
            }
        }
    }
}
