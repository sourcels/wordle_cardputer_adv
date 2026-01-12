#include "ui.h"
#include "words.h"
#include "anim.h"


volatile bool isWord = false;
bool lastIsWord = false;
unsigned long lastBatteryUpdate = 0;

char currentGuess[WORD_LENGTH + 1] = {0};
int currentGuessLen = 0;

int currentAttempt = 0;
char guesses[MAX_ATTEMPTS][WORD_LENGTH + 1];
uint8_t colors[MAX_ATTEMPTS][WORD_LENGTH];

volatile bool needRedraw = true;
volatile bool isChecking = false;
volatile bool needsEvaluation = false;
volatile bool gameOver = false;
volatile bool gameWon = false;

static M5Canvas sprite(&M5Cardputer.Display);

static const uint16_t COLOR_BG = TFT_BLACK;
static const uint16_t COLOR_BORDER = 0x4208;
static const uint16_t COLOR_TEXT = TFT_WHITE;
static const uint16_t COLOR_GREEN = TFT_GREEN;
static const uint16_t COLOR_YELLOW = TFT_YELLOW;
static const uint16_t COLOR_GRAY = 0x8410;
static const uint16_t COLOR_EMPTY = 0x2104;

void initUI() {
    M5Cardputer.Display.setRotation(1);
    M5Cardputer.Display.fillScreen(COLOR_BG);
    sprite.createSprite(M5Cardputer.Display.width(), M5Cardputer.Display.height());

    memset(guesses, 0, sizeof(guesses));
    memset(colors, 0, sizeof(colors));
    memset(currentGuess, 0, sizeof(currentGuess));
    currentGuessLen = 0;

    needRedraw = true;
}

void drawTile(int row, int col, char ch, uint8_t color) {
    int tileSize = 20;
    int margin = 2;
    int startX = 5;
    int startY = 5 + row * (tileSize + margin);
    int x = startX + col * (tileSize + margin);
    int y = startY;

    uint16_t fillColor;
    switch (color) {
        case 2: fillColor = COLOR_GREEN; break;
        case 1: fillColor = COLOR_YELLOW; break;
        default: fillColor = ch ? COLOR_GRAY : COLOR_EMPTY; break;
    }

    sprite.fillRoundRect(x, y, tileSize, tileSize, 3, fillColor);
    sprite.drawRoundRect(x, y, tileSize, tileSize, 3, COLOR_BORDER);

    if (ch) {
        sprite.setTextDatum(MC_DATUM);
        uint16_t textColor = (color == 1 || color == 2) ? TFT_BLACK : COLOR_TEXT;
        sprite.setTextColor(textColor, fillColor);
        sprite.setTextSize(1);
        char buf[2] = { (char)toupper(ch), '\0' };
        sprite.drawString(buf, x + tileSize / 2, y + tileSize / 2);
    }
}

void drawBattery() {
    int battLevel = M5Cardputer.Power.getBatteryLevel();

    int batW = 25;
    int batH = 10;
    int batX = sprite.width() - batW - 35;
    int batY = sprite.height() - 12;

    sprite.drawRect(batX, batY, batW, batH, TFT_WHITE);
    sprite.fillRect(batX + batW, batY + 3, 2, 4, TFT_WHITE);

    uint16_t batColor;
    if (battLevel > 60) batColor = COLOR_GREEN;
    else if (battLevel > 20) batColor = COLOR_YELLOW;
    else batColor = TFT_RED;

    int fillW = (batW - 4) * battLevel / 100;
    if (fillW > 0) {
        sprite.fillRect(batX + 2, batY + 2, fillW, batH - 4, batColor);
    }

    sprite.setTextDatum(ML_DATUM);
    sprite.setTextSize(1);
    sprite.setTextColor(TFT_WHITE, COLOR_BG);
    char batText[5];
    sprintf(batText, "%d%%", battLevel);
    sprite.drawString(batText, batX + batW + 5, batY + batH / 2);
}

void draw() {
    sprite.fillSprite(COLOR_BG);

    for (int r = 0; r < MAX_ATTEMPTS; r++) {
        for (int c = 0; c < WORD_LENGTH; c++) {
            char ch = guesses[r][c];
            uint8_t col = colors[r][c];
            drawTile(r, c, ch, col);
        }
    }

    if (currentAttempt < MAX_ATTEMPTS && !gameOver) {
        for (int c = 0; c < currentGuessLen; c++) {
            drawTile(currentAttempt, c, currentGuess[c], 0);
        }
         for (int c = currentGuessLen; c < WORD_LENGTH; c++) {
            drawTile(currentAttempt, c, 0, 0);
        }
    }

    int statusX = 150;
    int statusY = sprite.height() / 2 + 40;

    drawBattery();

    sprite.setTextDatum(TC_DATUM);
    sprite.setTextSize(2);
    sprite.setTextColor(TFT_WHITE, COLOR_BG);
    sprite.drawString("WORDLE", sprite.width() / 2 + 55, 10);

    sprite.setTextSize(1);
    if (gameOver) {
        sprite.setTextDatum(TC_DATUM);
        if (gameWon) {
            sprite.setTextColor(COLOR_GREEN, COLOR_BG);
            sprite.drawString("Correct", sprite.width() / 2 + 30, 30);
            sprite.drawString("guessed!", sprite.width() / 2 + 30, 42);
        } else {
            sprite.setTextColor(TFT_RED, COLOR_BG);
            sprite.drawString("No attempts", sprite.width() / 2 + 30, 30);
            sprite.drawString("left!", sprite.width() / 2 + 30, 42);
            
            sprite.setTextColor(COLOR_YELLOW, COLOR_BG);
            sprite.drawString("Word was:", sprite.width() / 2 + 30, 61);
            char upperWord[WORD_LENGTH + 1];
            for (int i = 0; i < WORD_LENGTH; i++) {
                upperWord[i] = toupper(targetWord[i]);
            }
            upperWord[WORD_LENGTH] = '\0';
            sprite.drawString(upperWord, sprite.width() / 2 + 30, 73);
        }
        
        sprite.setTextColor(TFT_WHITE, COLOR_BG);
        sprite.drawString("Press Enter", statusX, statusY - 12);
        sprite.drawString("to restart", statusX, statusY);

        sprite.pushSprite(0, 0);
        return;
    }

    if (isChecking) {
        sprite.setTextColor(COLOR_YELLOW, COLOR_BG);
        sprite.drawString("Checking", statusX + 15, statusY - 10);
        sprite.drawString("...", statusX, statusY);
    } else if (currentGuessLen == WORD_LENGTH) {
        if (isWord) {
            sprite.setTextColor(COLOR_GREEN, COLOR_BG);
            sprite.drawString("Valid!", statusX, statusY - 10);
            sprite.drawString("Press", statusX, statusY);
            sprite.drawString("[ok]", statusX, statusY + 10);
        } else if (needsEvaluation) {
            sprite.setTextColor(COLOR_YELLOW, COLOR_BG);
            sprite.drawString("Checking", statusX, statusY - 10);
            sprite.drawString("...", statusX, statusY);
        } else {
            sprite.setTextColor(TFT_RED, COLOR_BG);
            sprite.drawString("Invalid", statusX, statusY - 5);
            sprite.drawString("word!", statusX, statusY + 5);
        }
    } else if (currentGuessLen == 0) {
        sprite.setTextColor(TFT_DARKGREY, COLOR_BG);
        sprite.drawString("Type", statusX, statusY - 5);
        sprite.drawString("word", statusX, statusY + 5);
    }

    sprite.pushSprite(0, 0);
}

void handleKeyPress(char ch) {
    if (gameOver) {
        if (ch == '\n') {
            currentAttempt = 0;
            currentGuessLen = 0;
            memset(currentGuess, 0, sizeof(currentGuess));
            isWord = false;
            isChecking = false;
            needsEvaluation = false;
            gameOver = false;
            gameWon = false;
            memset(guesses, 0, sizeof(guesses));
            memset(colors, 0, sizeof(colors));

            extern volatile bool needNewWord;
            needNewWord = true;
            needRedraw = true;
        }
        return;
    }
    
    if (currentAttempt >= MAX_ATTEMPTS || isChecking) return;

    if (ch == '\b') {
        if (currentGuessLen > 0) {
            animateDelete(&sprite, currentAttempt, currentGuessLen - 1, currentGuess[currentGuessLen - 1], COLOR_EMPTY, COLOR_BORDER, COLOR_TEXT);

            currentGuessLen--;
            currentGuess[currentGuessLen] = '\0';

            isWord = false;
            needsEvaluation = false;
            needRedraw = true;
        }
    } else if (ch == '\n') {
        if (currentGuessLen == WORD_LENGTH && isWord) {
            strcpy(guesses[currentAttempt], currentGuess);
            memset(colors[currentAttempt], 0, WORD_LENGTH);

            memset(currentGuess, 0, sizeof(currentGuess));
            currentGuessLen = 0;
            
            isChecking = true;
            needsEvaluation = true;
            currentAttempt++;
            needRedraw = true;
        }
    } else if (isalpha(ch)) {
        if (currentGuessLen < WORD_LENGTH) {
            char letter = (char)tolower(ch);

            currentGuess[currentGuessLen] = letter;
            currentGuessLen++;
            currentGuess[currentGuessLen] = '\0';

            animateInput(&sprite, currentAttempt, currentGuessLen - 1, letter, COLOR_GRAY, COLOR_BORDER, COLOR_TEXT);
            
            needsEvaluation = false;
            needRedraw = true;
        }
    }
}

void awaitEval() {
    if (isChecking && !needsEvaluation) {
        isChecking = false;
        
        if (currentAttempt > 0) {
            int row = currentAttempt - 1;
            bool allGreen = true;
            for (int i = 0; i < WORD_LENGTH; i++) {
                if (colors[row][i] != 2) {
                    allGreen = false;
                    break;
                }
            }
            if (allGreen) {
                gameOver = true;
                gameWon = true;
            }
        }

        if (currentAttempt >= MAX_ATTEMPTS && !gameWon) {
            gameOver = true;
        }
        
        needRedraw = true;
    }
}

void Task_TFT(void *pvParameters) {
    initUI();

    while (true) {
        awaitEval();
        M5Cardputer.update();

        if (lastIsWord != isWord) {
            lastIsWord = isWord;
            needRedraw = true;
        }
        
        if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
            Keyboard_Class::KeysState ks = M5Cardputer.Keyboard.keysState();
            for (auto ch : ks.word) handleKeyPress(ch);
            if (ks.enter) handleKeyPress('\n');
            if (ks.del) handleKeyPress('\b');
        }

        unsigned long now = millis();
        if (now - lastBatteryUpdate > 5000) {
            lastBatteryUpdate = now;
            needRedraw = true;
        }

        if (needRedraw) {
            draw();
            needRedraw = false;
        }
        vTaskDelay(40 / portTICK_PERIOD_MS);
    }
}
