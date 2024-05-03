#ifndef TURING_MACHINE_H
#define TURING_MACHINE_H
#include <stdint.h>
#include <stdbool.h>

#define MAX_STATES 15
#define MAX_TRANSITIONS 10
#ifdef UNIT_TEST
#define TAPE_LENGTH 1000
#else
#define TAPE_LENGTH 300
#endif

#define EMPTY_TAPE_VALUE '_'

#define STARTING_STATE_INDEX 0
#define TERMINATING_STATE_INDEX 1

typedef struct {
    uint8_t nextState;
    char readSymbol;
    char writeSymbol;
    char moveDirection; // 'L' for left, 'R' for right
    uint8_t isUsed;
} Transition;

typedef struct {
    Transition transitions[MAX_TRANSITIONS];
    bool isUsed;
} State;

typedef struct {
    State states[MAX_STATES];
    char tape[TAPE_LENGTH];
    int tapePosition;
    uint8_t currentState;
    uint8_t numStates;
    uint8_t numTransitions;
    uint32_t calculationsCount;
} TuringMachine;

// Function prototypes
void initializeMachine(TuringMachine *tm);

void parseConfiguration(TuringMachine *tm, const char *config);

#endif // TURING_MACHINE_H
