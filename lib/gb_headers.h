#ifndef GB_HEADERS_H
#define GB_HEADERS_H
#include "turing_machine.h"

#define MAX_CONFIG_LENGTH 128
#define MAX_EXECUTIONS 65535

#define EXECUTION_MODE_INVALID 0
#define EXECUTION_MODE_STEP 1
#define EXECUTION_MODE_CALCULATION 2

typedef struct {
    char **configurations;
    uint8_t capacity;
    uint8_t size;
} ConfigurationsArray;

uint8_t displayMenu(TuringMachine *tm);

void runMachine(TuringMachine *tm, uint8_t executionMode);

#endif //GB_HEADERS_H
