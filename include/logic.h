#pragma once
#include <Arduino.h>


void Task_Logic(void *pvParameters);
bool wordExists(const char *guess);
void evaluateGuess(const char *guess, const char *target, uint8_t *colors);

extern volatile bool isChecking;
extern volatile bool needsEvaluation;
extern char currentGuess[];
extern int currentAttempt;