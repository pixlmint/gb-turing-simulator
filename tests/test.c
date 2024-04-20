#include <Basic.h>
#include <stdio.h>
#include <string.h>
#include "../lib/turing_logic.c"
#include "../lib/turing_init.c"

void printMachine(const TuringMachine tm) {
    printf("\n");
    for (int i = 0; i < MAX_STATES; i++) {
        if (tm.states[i].isUsed) {
            for (int j = 0; j < MAX_TRANSITIONS; j++) {
                if (tm.states[i].transitions[j].isUsed) {
                    printf("State %d, Symbol '%c': NextState: %d, WriteSymbol: '%c', MoveDirection: '%c'\n",
                           i,
                           tm.states[i].transitions[j].readSymbol,
                           tm.states[i].transitions[j].nextState,
                           tm.states[i].transitions[j].writeSymbol,
                           tm.states[i].transitions[j].moveDirection);
                }
            }
        }
    }

    printf("numStates: %d\n", tm.numStates);
    printf("numTransitions: %d\n", tm.numTransitions);
}

void assertMachineState(const Transition *transition, const int nextState, const char readSymbol,
                        const char writeSymbol, const char moveDirection) {
    printf("Asserting that nextState %d = %d, readSymbol %c = %c, writeSymbol %c = %c, moveDirection %c = %c\n",
           transition->nextState, nextState, transition->readSymbol, readSymbol, transition->writeSymbol, writeSymbol,
           transition->moveDirection, moveDirection);
    CU_ASSERT_EQUAL(transition->nextState, nextState);
    CU_ASSERT_EQUAL(transition->readSymbol, readSymbol);
    CU_ASSERT_EQUAL(transition->writeSymbol, writeSymbol);
    CU_ASSERT_EQUAL(transition->moveDirection, moveDirection);
}

void test_parseConfigurationSingularState(void) {
    TuringMachine tm;
    initializeMachine(&tm);

    const char *config =
            "0100100101001110";
    parseConfiguration(&tm, config);

    printMachine(tm);

    for (int i = 2; i < MAX_STATES; i++) {
        CU_ASSERT_EQUAL(tm.states[i].isUsed, 0);
    }

    CU_ASSERT_EQUAL(tm.states[0].isUsed, 1);
    CU_ASSERT_EQUAL(tm.states[1].isUsed, 1);

    assertMachineState(&tm.states[0].transitions[0], 1, '1', '0', 'R');
    CU_ASSERT_EQUAL(tm.numStates, 2);
    CU_ASSERT_EQUAL(tm.numTransitions, 1);
}

void test_parseConfigurationLeadingOne(void) {
    TuringMachine tm;
    initializeMachine(&tm);

    const char *config =
            "10100100101001110";
    parseConfiguration(&tm, config);

    printMachine(tm);

    for (int i = 2; i < MAX_STATES; i++) {
        CU_ASSERT_EQUAL(tm.states[i].isUsed, 0);
    }

    CU_ASSERT_EQUAL(tm.states[0].isUsed, 1);
    CU_ASSERT_EQUAL(tm.states[1].isUsed, 1);

    assertMachineState(&tm.states[0].transitions[0], 1, '1', '0', 'R');
    CU_ASSERT_EQUAL(tm.numStates, 2);
    CU_ASSERT_EQUAL(tm.numTransitions, 1);
}

void test_parseConfigurationFlipFlop(void) {
    TuringMachine tm;
    initializeMachine(&tm);

    const char *config =
            "010100101001100101010101110";
    parseConfiguration(&tm, config);

    printMachine(tm);

    for (int i = 2; i < MAX_STATES; i++) {
        CU_ASSERT_EQUAL(tm.states[i].isUsed, 0);
    }
    CU_ASSERT_EQUAL(tm.states[0].isUsed, 1);
    CU_ASSERT_EQUAL(tm.states[1].isUsed, 1);

    assertMachineState(&tm.states[0].transitions[0], 1, '0', '0', 'R');
    assertMachineState(&tm.states[1].transitions[0], 0, '0', '0', 'L');
    CU_ASSERT_EQUAL(tm.numStates, 2);
    CU_ASSERT_EQUAL(tm.numTransitions, 2);
}

void test_parseConfigurationThreeStates(void) {
    TuringMachine tm;
    initializeMachine(&tm);

    const char *config =
            "0101001010011001010101001101001000100101110";
    parseConfiguration(&tm, config);

    printMachine(tm);

    for (int i = 0; i < 3; i++) {
        CU_ASSERT_EQUAL(tm.states[i].isUsed, 1);
    }
    for (int i = 3; i < MAX_STATES; i++) {
        CU_ASSERT_EQUAL(tm.states[i].isUsed, 0);
    }

    assertMachineState(&tm.states[0].transitions[0], 1, '0', '0', 'R');
    assertMachineState(&tm.states[0].transitions[1], 2, '1', '1', 'L');
    assertMachineState(&tm.states[1].transitions[0], 0, '0', '0', 'R');

    CU_ASSERT_EQUAL(tm.numStates, 3);
    CU_ASSERT_EQUAL(tm.numTransitions, 3);
}

void test_parseConfigurationWebsiteExample(void) {
    TuringMachine tm;
    initializeMachine(&tm);

    const char *config =
            "0101000101001101001010010011000101000010100110001001010010011000010100001010011000010010000100100110000100010010001001111001";
    parseConfiguration(&tm, config);

    printMachine(tm);

    for (int i = 0; i < 4; i++) {
        CU_ASSERT_EQUAL(tm.states[i].isUsed, 1);
    }
    for (int i = 4; i < MAX_STATES; i++) {
        CU_ASSERT_EQUAL(tm.states[i].isUsed, 0);
    }

    CU_ASSERT_EQUAL(tm.tape[150], '1');
    CU_ASSERT_EQUAL(tm.tape[151], '0');
    CU_ASSERT_EQUAL(tm.tape[152], '0');
    CU_ASSERT_EQUAL(tm.tape[153], '1');

    assertMachineState(&tm.states[0].transitions[0], 2, '0', '0', 'R');
    assertMachineState(&tm.states[0].transitions[1], 0, '1', '1', 'R'); // recursive transition
    assertMachineState(&tm.states[2].transitions[0], 3, '0', '0', 'R');
    assertMachineState(&tm.states[2].transitions[1], 0, '1', '1', 'R');
    assertMachineState(&tm.states[3].transitions[0], 3, '0', '0', 'R');
    assertMachineState(&tm.states[3].transitions[1], 3, '1', '1', 'R');
    assertMachineState(&tm.states[3].transitions[2], 1, '_', '_', 'R');
}

void test_parseConfiguration(void) {
    test_parseConfigurationSingularState();
    test_parseConfigurationLeadingOne();
    test_parseConfigurationFlipFlop();
    test_parseConfigurationThreeStates();
    test_parseConfigurationWebsiteExample();
}

void test_parseDirection(void) {
    const char *direction1 = "0011";
    const char *direction2 = "011";
    const char *invalidDirection = "00011";
    CU_ASSERT_EQUAL('R', parseDirection(&direction1));
    CU_ASSERT_EQUAL('L', parseDirection(&direction2));
    CU_ASSERT_EQUAL('\0', parseDirection(&invalidDirection));
}

void test_parseBinaryValue(void) {
    const char *config1 = "0000";
    const char *config2 = "00001";
    const char *config3 = "1";
    int value;

    parseBinaryValue(&config1, &value);
    printf("\n1: %d, config: %s\n", value, config1);
    CU_ASSERT_EQUAL(value, 4);
    CU_ASSERT_STRING_EQUAL(config1, "");

    parseBinaryValue(&config2, &value);
    printf("2: %d, config: %s\n", value, config2);
    CU_ASSERT_EQUAL(value, 4);
    CU_ASSERT_STRING_EQUAL(config2, "1");

    parseBinaryValue(&config3, &value);
    printf("3: %d, config: %s\n", value, config3);
    CU_ASSERT_EQUAL(value, 0);
    CU_ASSERT_STRING_EQUAL(config3, "1");
}

void test_getApplicableTransitionEasy() {
    TuringMachine tm;
    initializeMachine(&tm);

    const char *config =
            "010100101001110";
    parseConfiguration(&tm, config);
    printMachine(tm);

    Transition t;
    getApplicableTransition(&tm, &t);

    assertMachineState(&t, 1, '0', '0', 'R');
}

void test_getApplicableTransitionHardcore() {
    TuringMachine tm;
    initializeMachine(&tm);

    const char *config =
            "0101000101001101001010010011000101000010100110001001010010011000010100001010011000010010000100100110000100010010001001111001";
    parseConfiguration(&tm, config);
    printMachine(tm);

    Transition t;

    tm.tape[tm.tapePosition] = '0';
    getApplicableTransition(&tm, &t);
    assertMachineState(&t, 2, '0', '0', 'R');

    tm.tape[tm.tapePosition] = '1';
    getApplicableTransition(&tm, &t);
    assertMachineState(&t, 0, '1', '1', 'R');

    tm.currentState = 2;
    tm.tape[tm.tapePosition] = '1';
    getApplicableTransition(&tm, &t);
    assertMachineState(&t, 0, '1', '1', 'R');

    tm.tape[tm.tapePosition] = '0';
    getApplicableTransition(&tm, &t);
    assertMachineState(&t, 3, '0', '0', 'R');

    tm.currentState = 3;
    tm.tape[tm.tapePosition] = '0';
    getApplicableTransition(&tm, &t);
    assertMachineState(&t, 3, '0', '0', 'R');

    tm.tape[tm.tapePosition] = '1';
    getApplicableTransition(&tm, &t);
    assertMachineState(&t, 3, '1', '1', 'R');

    tm.tape[tm.tapePosition] = '_';
    getApplicableTransition(&tm, &t);
    assertMachineState(&t, 1, '_', '_', 'R');
}

void test_getApplicableTransition(void) {
    test_getApplicableTransitionEasy();
    test_getApplicableTransitionHardcore();
}

void test_doMachineTurn(void) {
    TuringMachine tm;
    initializeMachine(&tm);

    const char *config =
            "010100101001110";
    parseConfiguration(&tm, config);
    printMachine(tm);

    doMachineTurn(&tm);

    CU_ASSERT_EQUAL(tm.currentState, 1);
    CU_ASSERT_EQUAL(tm.tapePosition, (TAPE_LENGTH / 2) + 1);
}

void test_countZerosBeforeOne(void) {
    const char *testval1 = "0011";
    const char *ptr1 = testval1;

    int zeros1Count = countZerosBeforeNextOne(&ptr1);
    printf("\ntestval: %s\n", ptr1);
    CU_ASSERT_EQUAL(zeros1Count, 2);
    CU_ASSERT_STRING_EQUAL(ptr1, "11");
}

void runMachineTest(TuringMachine *tm) {
    printMachine(*tm);

    printf("\n");
    while (doMachineTurn(tm) == 1) {
        printf("%s\n", tm->tape);
        for (int i = 0; i < tm->tapePosition; i++) {
            printf(".");
        }
        printf("^");
        for (int i = tm->tapePosition; i < TAPE_LENGTH; i++) {
            printf(".");
        }
        printf("\n");
    }
    printf("%s\n", tm->tape);
    for (int i = 0; i < tm->tapePosition; i++) {
        printf(".");
    }
    printf("^");
    for (int i = tm->tapePosition; i < TAPE_LENGTH; i++) {
        printf(".");
    }
    printf("\n");
    if (tm->currentState == 1) {
        printf("machine terminates\n");
    } else {
        printf("machine does not terminate, stuck on state %d\n", tm->currentState);
    }
}

void executeMachine(const char *config, int shouldTerminate) {
    TuringMachine tm;
    initializeMachine(&tm);

    parseConfiguration(&tm, config);
    runMachineTest(&tm);
    if (shouldTerminate) {
        CU_ASSERT_EQUAL(tm.currentState, 1);
    } else {
        CU_ASSERT_NOT_EQUAL(tm.currentState, 1);
    }
}

void test_machineExecution(void) {
    executeMachine("0101000101001101001010010011000101000010100110001001010010011000010100001010011000010010000100100110000100010010001001111001", 1);
    executeMachine("010010100100110101000101001100010010100100110001010010100111010", 0);
    executeMachine("0100101001001101010001010011000100101001001100010100101001110100", 1);
    executeMachine("010010000100100110000101000101001100001000100100001001100010010000100000100110001010101001111", 1);
    executeMachine("010010000100100110000101000101001100001000100100001001100010010000100000100110001010101001111001", 1);
    executeMachine("01001000010010011000010100010100110000100010010000100110001001000010000010011000101010100111101", 1);
    executeMachine("010010000100100110000101000101001100001000100100001001100010010000100000100110001010101001111010", 0);
}

void test_getSymbolFromValue() {
    const char tapeCharacters[] = {'0', '1', '_', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
    int charactersCount = sizeof(tapeCharacters) / sizeof(tapeCharacters[0]);

    for (int i = 0; i < charactersCount; i++) {
        printf("Validating that calculated symbol %c == %c\n", getSymbolFromValue(i), tapeCharacters[i]);
        CU_ASSERT_EQUAL(getSymbolFromValue(i), tapeCharacters[i]);
    }

    CU_ASSERT_EQUAL(getSymbolFromValue(charactersCount + 1), '_');
    CU_ASSERT_EQUAL(getSymbolFromValue(charactersCount + 2), '_');
}

void test_helpers(void) {
    test_getSymbolFromValue();
}

int main() {
    CU_initialize_registry();

    CU_pSuite helpersSuite = CU_add_suite("helpersTest", NULL, NULL);
    CU_add_test(helpersSuite, "test_helpers", test_helpers);

    CU_pSuite parseConfigurationSuite = CU_add_suite("parseConfiguration_test", NULL, NULL);
    CU_add_test(parseConfigurationSuite, "test_parseConfiguration", test_parseConfiguration);

    CU_pSuite parseDirectionSuite = CU_add_suite("parseConfiguration_test", NULL, NULL);
    CU_add_test(parseDirectionSuite, "test_parseConfiguration", test_parseDirection);

    CU_pSuite parseBinaryValueSuite = CU_add_suite("parseBinaryValue_test", NULL, NULL);
    CU_add_test(parseBinaryValueSuite, "test_parseBinaryValue", test_parseBinaryValue);

    CU_pSuite countZerosBeforeOneSuite = CU_add_suite("countZerosBeforeOne_test", NULL, NULL);
    CU_add_test(countZerosBeforeOneSuite, "test_countZerosBeforeOne", test_countZerosBeforeOne);

    CU_pSuite getApplicableTransitionSuite = CU_add_suite("getApplicableTransition_test", NULL, NULL);
    CU_add_test(getApplicableTransitionSuite, "test_getApplicableTransition", test_getApplicableTransition);

    CU_pSuite doMachineTurnSuite = CU_add_suite("doMachineTurn_test", NULL, NULL);
    CU_add_test(doMachineTurnSuite, "test_doMachineTurn", test_doMachineTurn);

    CU_pSuite machineExecutionSuite = CU_add_suite("machineExecution_test", NULL, NULL);
    CU_add_test(machineExecutionSuite, "test_machineExecution", test_machineExecution);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    return CU_get_error();
}
