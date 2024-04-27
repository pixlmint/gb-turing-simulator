#include <stdint.h>
#include "turing_machine.h"
#include "gb_headers.h"
// #include <types.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

ConfigurationsArray *createArray(const uint8_t initialSize) {
    ConfigurationsArray *arr = malloc(sizeof(ConfigurationsArray));
    arr->configurations = malloc(initialSize * sizeof(char *));
    arr->capacity = initialSize;
    arr->size = 0;
    return arr;
}

void addConfiguration(ConfigurationsArray *arr, const char *configuration) {
    if (arr->size == arr->capacity) {
        // Resize the array if it's full
        const uint8_t newCapacity = arr->capacity * 2;
        char **newStrings = realloc(arr->configurations, newCapacity * sizeof(char *));
        if (newStrings == NULL) {
            printf("Failed to allocate memory.\n");
            return;
        }
        arr->configurations = newStrings;
        arr->capacity = newCapacity;
    }
    // Allocate memory for the new string and add it to the array
    arr->configurations[arr->size] = malloc(strlen(configuration) + 1);
    strcpy(arr->configurations[arr->size], configuration);
    arr->size++;
}

// Function to free the array
void freeArray(ConfigurationsArray *arr) {
    for (uint8_t i = 0; i < arr->size; i++) {
        free(arr->configurations[i]);
    }
    free(arr->configurations);
    free(arr);
}
