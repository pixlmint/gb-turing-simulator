#ifndef GB_HEADERS_H
#define GB_HEADERS_H
#include "turing_machine.h"

#define MAX_CONFIG_LENGTH 128
#define MAX_EXECUTIONS 1000

#define EXECUTION_MODE_STEP 1
#define EXECUTION_MODE_CALCULATION 2

void displayMenu(TuringMachine *tm);

#endif //GB_HEADERS_H
