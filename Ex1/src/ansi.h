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

#endif /* ANSI_H */
