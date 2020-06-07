#ifndef ANSI_H
#define ANSI_H

#include <stdint.h>
#include <stdio.h>

void fgcolor(uint8_t foreground);
void bgcolor(uint8_t background);
void color(uint8_t foreground, uint8_t background);
void resetbgcolor();
void clearTermninal();
void clreol();
void gotoxy(uint8_t x, uint8_t y);
void homeCurser();
void underline (uint8_t on);
void blink(uint8_t on);
void inverse(uint8_t on);
setCharColor(uint8_t color);
#endif /* ANSI_H */
