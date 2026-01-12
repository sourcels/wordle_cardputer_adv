#include "anim.h"


void drawFrame(M5Canvas *sprite, int x, int y, int size, char letter, float scale, uint16_t bg, uint16_t border, uint16_t text) {
    sprite->fillRoundRect(x, y, size, size, 3, bg);
    sprite->drawRoundRect(x, y, size, size, 3, border);

    if (letter != 0 && scale > 0.1) {
        sprite->setTextDatum(MC_DATUM);
        sprite->setTextColor(text, bg);

        sprite->setTextSize(scale); 
        
        char buf[2] = { (char)toupper(letter), '\0' };
        sprite->drawString(buf, x + size / 2, y + size / 2);
    }

    sprite->pushSprite(0, 0);
}

void animateInput(M5Canvas *sprite, int row, int col, char letter, uint16_t colorBg, uint16_t colorBorder, uint16_t colorText) {
    int tileSize = 20;
    int margin = 2;
    int startX = 5;
    int startY = 5 + row * (tileSize + margin);
    int x = startX + col * (tileSize + margin);

    float scales[ANIM_FRAMES] = {0.5, 1.2, 1.4, 1.1, 1.0};

    for (int i = 0; i < ANIM_FRAMES; i++) {
        drawFrame(sprite, x, startY, tileSize, letter, scales[i], colorBg, colorBorder, colorText);
        delay(ANIM_DELAY);
    }
}

void animateDelete(M5Canvas *sprite, int row, int col, char letter, uint16_t colorBg, uint16_t colorBorder, uint16_t colorText) {
    int tileSize = 20;
    int margin = 2;
    int startX = 5;
    int startY = 5 + row * (tileSize + margin);
    int x = startX + col * (tileSize + margin);

    float scales[ANIM_FRAMES] = {1.0, 0.8, 0.5, 0.2, 0.0};

    for (int i = 0; i < ANIM_FRAMES; i++) {
        drawFrame(sprite, x, startY, tileSize, letter, scales[i], colorBg, colorBorder, colorText);
        delay(ANIM_DELAY);
    }

    drawFrame(sprite, x, startY, tileSize, 0, 0, colorBg, colorBorder, colorText);
}