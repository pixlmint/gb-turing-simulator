#include <Basic.h>
#include <stdio.h>
#include <string.h>
#include "../lib/turing_logic.c"
#include "../lib/turing_init.c"
#include "../lib/dyn_array.c"
#include <sys/time.h>

#define EXECUTION_PRINT_STEPS 0

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

    const char config[] = {'0', '1', '0', '0', '1', '0', '0', '1', '0', '1', '0', '0', '1', '1', '1', '0'};
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

    CU_ASSERT_EQUAL(tm.tape[0], '1');
    CU_ASSERT_EQUAL(tm.tape[1], '0');
    CU_ASSERT_EQUAL(tm.tape[2], '0');
    CU_ASSERT_EQUAL(tm.tape[3], '1');

    assertMachineState(&tm.states[0].transitions[0], 2, '0', '0', 'R');
    assertMachineState(&tm.states[0].transitions[1], 0, '1', '1', 'R'); // recursive transition
    assertMachineState(&tm.states[2].transitions[0], 3, '0', '0', 'R');
    assertMachineState(&tm.states[2].transitions[1], 0, '1', '1', 'R');
    assertMachineState(&tm.states[3].transitions[0], 3, '0', '0', 'R');
    assertMachineState(&tm.states[3].transitions[1], 3, '1', '1', 'R');
    assertMachineState(&tm.states[3].transitions[2], 1, '_', '_', 'R');
}

void test_tapeShift() {
    TuringMachine tm;
    initializeMachine(&tm);

    const char *machineConfiguration = "0100010100010111";
    parseConfiguration(&tm, machineConfiguration);

    tm.tapePosition = TAPE_LENGTH - 1;
    CU_ASSERT_EQUAL(doMachineTurn(&tm), 1);

    CU_ASSERT_EQUAL(tm.tapePosition, TAPE_LENGTH - 2);

    tm.tapePosition = 0;
    CU_ASSERT_EQUAL(doMachineTurn(&tm), 1);

    CU_ASSERT_EQUAL(tm.tapePosition, 0);
}

void test_tapeBehaviour(void) {
    test_tapeShift();
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
    char *config1 = "0000";
    char *config2 = "00001";
    char *config3 = "1";
    uint8_t value;

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
    const int tapeStartingPosition = tm.tapePosition;
    parseConfiguration(&tm, config);
    printMachine(tm);

    doMachineTurn(&tm);

    CU_ASSERT_EQUAL(tm.currentState, 1);
    CU_ASSERT_EQUAL(tm.tapePosition, tapeStartingPosition + 1);
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

    struct timeval stop, start;
    gettimeofday(&start, NULL);

    if (EXECUTION_PRINT_STEPS == 1) {
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
    } else {
        while (doMachineTurn(tm) == 1) {
        }
    }

    gettimeofday(&stop, NULL);
    printf("\n");
    float executionTime = (stop.tv_usec - start.tv_usec) / 1000;
    printf("execution time (in millis):\t%f\n", executionTime);
    if (tm->currentState == 1) {
        printf("machine terminates\n");
        printf("num executions: %d\n", tm->calculationsCount);
    } else {
        printf("machine does not terminate, stuck on state %d\n", tm->currentState);
    }
}

void executeMachine(const char *config, const int shouldTerminate) {
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

void executeMachineAndVerifyTape(const char *config, const bool shouldTerminate, const char *expectedTape) {
    TuringMachine tm;
    initializeMachine(&tm);

    parseConfiguration(&tm, config);
    runMachineTest(&tm);
    if (shouldTerminate) {
        CU_ASSERT_EQUAL(tm.currentState, 1);
    } else {
        CU_ASSERT_NOT_EQUAL(tm.currentState, 1);
    }

    const int expectedTapeLength = strlen(expectedTape);

    // find machine tape first character
    int machineFirstCharacter = -1;
    for (int i = 0; i < TAPE_LENGTH; i++) {
        if (tm.tape[i] != '_') {
            machineFirstCharacter = i;
            break;
        }
    }

    bool isTapeValid = 1;
    bool hasTapeExtraCharacters = 0;
    for (int i = 0; i < TAPE_LENGTH; i++) {
        const char characterToCheck = tm.tape[i];
        const char expectedCharacter = expectedTape[i - machineFirstCharacter];
        if (i >= machineFirstCharacter && i <= expectedTapeLength) {
            if (characterToCheck != expectedCharacter && expectedCharacter != '\0') {
                printf("\ni: %d, t: %c, e: %c", i, characterToCheck, expectedCharacter);
                isTapeValid = 0;
            }
        } else if (characterToCheck != EMPTY_TAPE_VALUE && characterToCheck != '\0') {
            hasTapeExtraCharacters = 1;
            isTapeValid = 0;
            printf("\ni: %d, t: %c", i, characterToCheck);
        }
    }

    CU_ASSERT_TRUE(isTapeValid);
    CU_ASSERT_FALSE(hasTapeExtraCharacters);

    if (!isTapeValid) {
        printf("\nThe tape is invalid");
    }
    if (hasTapeExtraCharacters) {
        printf("\nThe tape contains unexpected values");
    }
    if (!isTapeValid) {
        printf("\n%s", tm.tape);
    }
}

void test_machineExecution(void) {
    executeMachine(
        "0101000101001101001010010011000101000010100110001001010010011000010100001010011000010010000100100110000100010010001001111001",
        1);
    executeMachine("010010100100110101000101001100010010100100110001010010100111010", 0);
    executeMachine("0100101001001101010001010011000100101001001100010100101001110100", 1);
    executeMachine("010010000100100110000101000101001100001000100100001001100010010000100000100110001010101001111", 1);
    executeMachine("010010000100100110000101000101001100001000100100001001100010010000100000100110001010101001111001",
                   1);
    executeMachine("01001000010010011000010100010100110000100010010000100110001001000010000010011000101010100111101",
                   1);
    executeMachine("010010000100100110000101000101001100001000100100001001100010010000100000100110001010101001111010",
                   0);
    //executeMachine("0100010100100111_", 0); // infinite execution
    executeMachineAndVerifyTape(
        "0100010010001001100100010001001001100010010001001001100010001000010001001100001000100001001011000010010010010111_",
        1, "111111");
    executeMachineAndVerifyTape(
        "01010101001101001010010011010001000100000001001100010100010100110001001000100100110001000100001000101100001001000001000010110000010100000101011000001001000001001011000001000000010000001000000010110000001000001000000100000101100000010000001000000100000010110000001010000000100000010011000000100010000000100000010011000000010100000001010011000000010010000000100100110000000100000100000001000001001100000001000000100000001000000100110000000100000001000000010000000100110000001001000000001000001011000000000100100000000010101100000000010100000001001001100000000010001000000010010011000000010000100001001011000010100000000001000010110000000000101000000000010101100000000001001000000000010010110000000000100000001000000000001000000010110000000000010000001000000000001000000101100000000000100000100000000000100000101100000000000101000000000000100000100110000000000010001000000000000100000100110000000000010010000000000001000000100110000000000001010000000000001010011000000000000100100000000000010010011000000000000100000100000000000010000010011000000000000100000010000000000001000000100110000000000001000000010000000000001000000010011000000000000100001000010101100001000000010000000000000100010110000000000000101000000000000010101100000000000001001000000000000010010110000000000000100000010000000000000100101100000000000001000001000000000000010101100000000000001000100100010011110_100",
        1, "110_100");
    executeMachineAndVerifyTape(
        "010101010011010010100100110100010001000000010011000101000101001100010010001001001100010001000010001011000010010000010000101100000101000001010110000010010000010010110000010000000100000010000000101100000010000010000001000001011000000100000010000001000000101100000010100000001000000100110000001000100000001000000100110000000101000000010100110000000100100000001001001100000001000001000000010000010011000000010000001000000010000001001100000001000000010000000100000001001100000010010000000010000010110000000001001000000000101011000000000101000000010010011000000000100010000000100100110000000100001000010010110000101000000000010000101100000000001010000000000101011000000000010010000000000100101100000000001000000010000000000010000000101100000000000100000010000000000010000001011000000000001000001000000000001000001011000000000001010000000000001000001001100000000000100010000000000001000001001100000000000100100000000000010000001001100000000000010100000000000010100110000000000001001000000000000100100110000000000001000001000000000000100000100110000000000001000000100000000000010000001001100000000000010000000100000000000010000000100110000000000001000010000101011000010000000100000000000001000101100000000000001010000000000000101011000000000000010010000000000000100101100000000000001000000100000000000001001011000000000000010000010000000000000101011000000000000010001001000100111101_1000",
        1, "1101_1000");
    executeMachineAndVerifyTape("010001000100000000000100110001000100001000000001001100001000100000100000000000000010011000001000100000010000000000000001001100000010001000000010000000000000000001001100000001000100000000100010011000000001000100000000010000000000000000000000000010011000000000100010000000000100000000000000000010011000000000010001000000000001000000000000000000000100110000000000010001000000000000100000000000000010011000000000000100010010000000100111_",
                                1, "hello_world");
    executeMachineAndVerifyTape("01000100100010011010010001001001101001000010000101100100000100100010011000100100000000001001011000100010010001001100001001000010010110000100001000010000101100001000000100001000000101100001000100000100100110000010010000010010011000001000010000001000010011000001000000100000010000001001100000010000100000010000100110000001000000100000010000001001100000010010000100001011000000100000100001000000101100000010001000000010001011000000010000001000000010000010110000000100001000000001000001011000000001000010000000010010110000000010010000000001001001100000000010010000100001011000000000100000100100010011000000000010010000100001011111",
                                1, "1111");
    printf("\n2^3\t9\n");
    executeMachineAndVerifyTape("01000100100010011010010001001001101001000010000101100100000100100010011001000100100010110001001000000000010010110001000100100010011000010010000100101100001000010000100001011000010000001000010000001011000010001000001001001100000100100000100100110000010000100000010000100110000010000001000000100000010011000000100001000000100001001100000010000001000000100000010011000000100100001000010110000001000001000010000001011000000100010000000100010110000000100000010000000100000101100000001000010000000010000010110000000010000100000000100101100000000100100000000010010011000000000100100001000010110000000001000001001000100110000000000100100001000010111111",1, "111111111");
    printf("\n2^5\t25\n");
    executeMachineAndVerifyTape("0100010010001001101001000100100110100100001000010110010000010010001001100100010010001011000100100000000001001011000100010010001001100001001000010010110000100001000010000101100001000000100001000000101100001000100000100100110000010010000010010011000001000010000001000010011000001000000100000010000001001100000010000100000010000100110000001000000100000010000001001100000010010000100001011000000100000100001000000101100000010001000000010001011000000010000001000000010000010110000000100001000000001000001011000000001000010000000010010110000000010010000000001001001100000000010010000100001011000000000100000100100010011000000000010010000100001011111111",1, "1111111111111111111111111");
    printf("\n2^10\t100\n");
    executeMachineAndVerifyTape("010001001000100110100100010010011010010000100001011001000001001000100110010001001000101100010010000000000100101100010001001000100110000100100001001011000010000100001000010110000100000010000100000010110000100010000010010011000001001000001001001100000100001000000100001001100000100000010000001000000100110000001000010000001000010011000000100000010000001000000100110000001001000010000101100000010000010000100000010110000001000100000001000101100000001000000100000001000001011000000010000100000000100000101100000000100001000000001001011000000001001000000000100100110000000001001000010000101100000000010000010010001001100000000001001000010000101111111111111", 1, "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111");
    printf("\n2^20\t400\n");
    executeMachineAndVerifyTape("0100010010001001101001000100100110100100001000010110010000010010001001100100010010001011000100100000000001001011000100010010001001100001001000010010110000100001000010000101100001000000100001000000101100001000100000100100110000010010000010010011000001000010000001000010011000001000000100000010000001001100000010000100000010000100110000001000000100000010000001001100000010010000100001011000000100000100001000000101100000010001000000010001011000000010000001000000010000010110000000100001000000001000001011000000001000010000000010010110000000010010000000001001001100000000010010000100001011000000000100000100100010011000000000010010000100001011111111111111111111111", 1, "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111");
    // executeMachineAndVerifyTape( usquareConfig + "1111111111111111111111111111", 1, "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111");
}

void test_getSymbolFromValue() {
    const char tapeCharacters[] = {
        '0', '1', '_', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's',
        't', 'u', 'v', 'w', 'x', 'y', 'z'
    };
    const int charactersCount = sizeof(tapeCharacters) / sizeof(tapeCharacters[0]);

    for (int i = 0; i < charactersCount; i++) {
        printf("Validating that calculated symbol %c == %c\n", getSymbolFromValue(i), tapeCharacters[i]);
        CU_ASSERT_EQUAL(getSymbolFromValue(i), tapeCharacters[i]);
    }

    CU_ASSERT_EQUAL(getSymbolFromValue(charactersCount + 1), '_');
    CU_ASSERT_EQUAL(getSymbolFromValue(charactersCount + 2), '_');
}

void test_helpers(void) {
    test_parseDirection();
    test_getSymbolFromValue();
}

void test_createArray(void) {
    ConfigurationsArray *arr = createArray(10);
    CU_ASSERT_PTR_NOT_NULL(arr);
    CU_ASSERT_PTR_NOT_NULL(arr->configurations);
    CU_ASSERT_EQUAL(arr->capacity, 10);
    CU_ASSERT_EQUAL(arr->size, 0);
    freeArray(arr);
}

void test_addConfiguration(void) {
    ConfigurationsArray *arr = createArray(2);
    addConfiguration(arr, "config1");
    CU_ASSERT_STRING_EQUAL(arr->configurations[0], "config1");
    CU_ASSERT_EQUAL(arr->size, 1);
    addConfiguration(arr, "config2");
    CU_ASSERT_STRING_EQUAL(arr->configurations[1], "config2");
    CU_ASSERT_EQUAL(arr->size, 2);
    addConfiguration(arr, "config3");
    CU_ASSERT_STRING_EQUAL(arr->configurations[2], "config3");
    CU_ASSERT_EQUAL(arr->size, 3);
    CU_ASSERT_EQUAL(arr->capacity, 4);
    freeArray(arr);
}

void test_freeArray(void) {
    ConfigurationsArray *arr = createArray(2);
    addConfiguration(arr, "config1");
    addConfiguration(arr, "config2");
    freeArray(arr);
    // As the memory is freed, we can't make assertions on the content of the array.
    // But we can assert that the program doesn't crash.
}

void test_dynArray(void) {
    test_createArray();
    test_addConfiguration();
    test_freeArray();
}

int main() {
    CU_initialize_registry();

    const CU_pSuite helpersSuite = CU_add_suite("helpersTest", NULL, NULL);
    CU_add_test(helpersSuite, "test_helpers", test_helpers);

    const CU_pSuite parseConfigurationSuite = CU_add_suite("parseConfiguration_test", NULL, NULL);
    CU_add_test(parseConfigurationSuite, "test_parseConfiguration", test_parseConfiguration);

    const CU_pSuite parseBinaryValueSuite = CU_add_suite("parseBinaryValue_test", NULL, NULL);
    CU_add_test(parseBinaryValueSuite, "test_parseBinaryValue", test_parseBinaryValue);

    const CU_pSuite countZerosBeforeOneSuite = CU_add_suite("countZerosBeforeOne_test", NULL, NULL);
    CU_add_test(countZerosBeforeOneSuite, "test_countZerosBeforeOne", test_countZerosBeforeOne);

    const CU_pSuite getApplicableTransitionSuite = CU_add_suite("getApplicableTransition_test", NULL, NULL);
    CU_add_test(getApplicableTransitionSuite, "test_getApplicableTransition", test_getApplicableTransition);

    const CU_pSuite doMachineTurnSuite = CU_add_suite("doMachineTurn_test", NULL, NULL);
    CU_add_test(doMachineTurnSuite, "test_doMachineTurn", test_doMachineTurn);

    const CU_pSuite machineExecutionSuite = CU_add_suite("machineExecution_test", NULL, NULL);
    CU_add_test(machineExecutionSuite, "test_machineExecution", test_machineExecution);

    const CU_pSuite tapeBehaviourSuite = CU_add_suite("tapeBehaviour_test", NULL, NULL);
    CU_add_test(tapeBehaviourSuite, "test_tapeBehaviour", test_tapeBehaviour);

    const CU_pSuite dynArraySuite = CU_add_suite("dynArray_test", NULL, NULL);
    CU_add_test(dynArraySuite, "test_dynArray", test_dynArray);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    return CU_get_error();
}
