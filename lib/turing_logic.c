#include "turing_machine.h"
#include <stdint.h>

bool getApplicableTransition(const TuringMachine *tm, Transition *applicableTransition) {
    const char tapeValue = tm->tape[tm->tapePosition];
    for (uint8_t i = 0; i < MAX_TRANSITIONS; i++) {
        Transition transition = {0};
        transition = tm->states[tm->currentState].transitions[i];
        if (transition.isUsed == 0) {
            continue;
        }
        if (transition.readSymbol == tapeValue) {
            *applicableTransition = transition;
            return 1;
        }
    }
    return 0;
}

void shiftTapeIfNeeded(TuringMachine *tm) {
    if (tm->tapePosition < 0) {
        // Shift all elements to the right
        for (int i = TAPE_LENGTH - 1; i > 0; i--) {
            tm->tape[i] = tm->tape[i - 1];
        }
        // Set the first element to the empty tape value
        tm->tape[0] = EMPTY_TAPE_VALUE;
        // Adjust the tape position
        tm->tapePosition = 0;
    }
    if (tm->tapePosition >= TAPE_LENGTH) {
        // Shift all elements to the left
        for (uint16_t i = 0; i < TAPE_LENGTH - 1; i++) {
            tm->tape[i] = tm->tape[i + 1];
        }
        // Set the last element to the empty tape value
        tm->tape[TAPE_LENGTH - 1] = EMPTY_TAPE_VALUE;
        // Adjust the tape position
        tm->tapePosition = TAPE_LENGTH - 1;
    }
}

bool doMachineTurn(TuringMachine *tm) {
    Transition applicableTransition;
    const bool foundATransition = getApplicableTransition(tm, &applicableTransition);
    if (foundATransition == 0) {
        return 0;
    }
    tm->tape[tm->tapePosition] = applicableTransition.writeSymbol;
    tm->currentState = applicableTransition.nextState;
    if (applicableTransition.moveDirection == 'R') {
        tm->tapePosition++;
    } else {
        tm->tapePosition--;
    }
    tm->calculationsCount += 1;
    shiftTapeIfNeeded(tm);
    return 1;
}
