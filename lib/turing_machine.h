#ifndef TURING_MACHINE_H
#define TURING_MACHINE_H

#define MAX_STATES 10
#define MAX_TRANSITIONS 10
#define TAPE_LENGTH 300

#define EMPTY_TAPE_VALUE '_'

#define STARTING_STATE_INDEX 0
#define TERMINATING_STATE_INDEX 1

typedef struct {
    int nextState;
    char readSymbol;
    char writeSymbol;
    char moveDirection; // 'L' for left, 'R' for right
    int isUsed;
} Transition;

typedef struct {
    Transition transitions[MAX_TRANSITIONS];
    int isUsed;
} State;

typedef struct {
    State states[MAX_STATES];
    char tape[TAPE_LENGTH];
    int tapePosition;
    int currentState;
    int numStates;
    int numTransitions;
} TuringMachine;

// Function prototypes
void initializeMachine(TuringMachine *tm);

void parseConfiguration(TuringMachine *tm, const char *config);

int runMachine(TuringMachine *tm);

#endif // TURING_MACHINE_H
