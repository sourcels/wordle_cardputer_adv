#pragma once
#include <M5Cardputer.h>


#define ANIM_FRAMES 5
#define ANIM_DELAY 15

void animateInput(M5Canvas *sprite, int row, int col, char letter, uint16_t colorBg, uint16_t colorBorder, uint16_t colorText);

void animateDelete(M5Canvas *sprite, int row, int col, char letter, uint16_t colorBg, uint16_t colorBorder, uint16_t colorText);