#include "logic.h"
#include "ui.h"
#include "words.h"

#include <string.h>
#include <stdlib.h>

char targetWord[WORD_LENGTH + 1];
volatile bool needNewWord = false;

bool wordExists(const char *guess) {
    int low = 0, high = WORD_COUNT - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        int cmp = strcmp(guess, wordList[mid]);
        if (cmp == 0) return true;
        if (cmp < 0) high = mid - 1;
        else low = mid + 1;
    }
    return false;
}

void evaluateGuess(const char *guess, const char *target, uint8_t *colors) {
    bool used[WORD_LENGTH] = {false};

    for (int i = 0; i < WORD_LENGTH; i++) {
        if (guess[i] == target[i]) {
            colors[i] = 2;
            used[i] = true;
        } else {
            colors[i] = 0;
        }
    }

    for (int i = 0; i < WORD_LENGTH; i++) {
        if (colors[i] == 0) {
            for (int j = 0; j < WORD_LENGTH; j++) {
                if (!used[j] && guess[i] == target[j]) {
                    colors[i] = 1;
                    used[j] = true;
                    break;
                }
            }
        }
    }
}

void Task_Logic(void *pvParameters) {
    srand(millis());
    strcpy(targetWord, wordList[rand() % WORD_COUNT]);

    String lastChecked = "";
    
    while (true) {
        if (needNewWord) {
            strcpy(targetWord, wordList[rand() % WORD_COUNT]);
            needNewWord = false;
        }

        if (currentGuess.length() > 0 && currentGuess != lastChecked) {
            lastChecked = currentGuess;
            
            if (currentGuess.length() == WORD_LENGTH) {
                char tempWord[WORD_LENGTH + 1];
                strcpy(tempWord, currentGuess.c_str());
                isWord = wordExists(tempWord);
            } else {
                isWord = false;
            }
        }

        if (needsEvaluation && currentAttempt > 0) {
            int row = currentAttempt - 1;
            const char *word = guesses[row];

            if (wordExists(word)) {
                evaluateGuess(word, targetWord, colors[row]);
            }
            
            needsEvaluation = false;
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
