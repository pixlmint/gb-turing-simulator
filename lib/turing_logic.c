#include "turing_machine.h"

int getApplicableTransition(TuringMachine *tm, Transition *applicableTransition) {
    const char tapeValue = tm->tape[tm->tapePosition];
    for (int i = 0; i < MAX_TRANSITIONS; i++) {
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

int doMachineTurn(TuringMachine *tm) {
    Transition applicableTransition;
    int foundATransition = getApplicableTransition(tm, &applicableTransition);
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
    return 1;
}
