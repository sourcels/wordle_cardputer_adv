#pragma once
#include <Arduino.h>
#include "M5Cardputer.h"

#define WORD_LENGTH 5
#define MAX_ATTEMPTS 6

extern bool isWord;
extern bool needRedraw;
extern String currentGuess;
extern int currentAttempt;
extern char guesses[MAX_ATTEMPTS][WORD_LENGTH + 1];
extern uint8_t colors[MAX_ATTEMPTS][WORD_LENGTH];

extern volatile bool gameOver;
extern volatile bool gameWon;
extern char targetWord[WORD_LENGTH + 1];

extern volatile bool isChecking;
extern volatile bool needsEvaluation;

void Task_TFT(void *pvParameters);