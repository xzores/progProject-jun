/*
**
**                           Main.c
**
**
**********************************************************************/
/*
   Last committed:     $Revision: 00 $
   Last changed by:    $Author: $
   Last changed date:  $Date:  $
   ID:                 $Id:  $

**********************************************************************/
#include "stm32f30x_conf.h"
#include <stdlib.h>
#include "30010_io.h"
#include "Utility.h"

struct Map{

    uint8_t* buffer;
    uint16_t mySize;
    char style[4][3];
    uint8_t colors[4];
};


void buildMap(struct Map* myMap, uint8_t x, uint8_t y, uint8_t sizeX, uint8_t sizeY, char style) {

    uint16_t i, j;
    uint8_t type;

    if (style == 'R') {
        type = 0;
    } else if(style == 'W') {
        type = 1;
    } else if (style == 'G') {
        type = 2;
    } else {
        type = 3;
    }


    for (i = x; i < x + sizeX; i++) {
        for (j = y; j < y + sizeY; j++) {
            uint8_t shiftIndex = (i % 4) * 2;
            uint16_t index = i / 4;
            myMap->buffer[index /*% myMap->mySize*/ + j * myMap->mySize] &= ~(0x3 << shiftIndex); //tile resettes //index % myMap->mySize er altid lig index ??
            myMap->buffer[index /*% myMap->mySize*/ + j * myMap->mySize] |= (type << shiftIndex);
        }
    };
};


struct visual{
    uint8_t digit1, digit2, digit3, digit4, inv;
} default_visual = {'3','7','4','0',0}; //ESC[37m ESC[40m, reset to white foreground, black background, inv off

uint8_t tileScheme(char* toPrint, uint8_t t, uint8_t style) {
    struct visual myScheme;
    myScheme = default_visual;

    if (style == 0) { //Road, BG White, inv Off
        myScheme.digit1 = '3';
        myScheme.digit2 = '7';

    } else if (style == 1) { //Wall, FG 90, BG, 40, INV ON
        myScheme.digit1 = '9';
        myScheme.digit2 = '0';
        myScheme.digit3 = '4';
        myScheme.digit4 = '0';
        myScheme.inv = 1;
    } else if (style == 2) { //Grass, FG 92, BG 42, INV Off
        myScheme.digit1 = '9';
        myScheme.digit2 = '2';
        myScheme.digit3 = '4';
        myScheme.digit4 = '2';
     } else if (style == 3) { //Lygtepæl, FG 93, BG 43, INv 0ff
        myScheme.digit1 = '9';
        myScheme.digit2 = '3';
        myScheme.digit3 = '4';
        myScheme.digit4 = '3';
     }

     //Invers printet to array
     toPrint[t++] = 0x1B; // ESC
     toPrint[t++] = 0x5B; // [
     if (myScheme.inv == 0) {
         toPrint[t++] = '2'; } //7 eller 27, on eller off
     toPrint[t++] = '7'; //7 for inverse, 27 for not-inverse
     toPrint[t++] = 'm';


    //Foreground og Background
    toPrint[t++] = 0x1B; // ESC
    toPrint[t++] = 0x5B; // [
    toPrint[t++] = myScheme.digit1;
    toPrint[t++] = myScheme.digit2;
    toPrint[t++] = 'm';
    toPrint[t++] = 0x1B; // ESC
    toPrint[t++] = 0x5B; // [
    toPrint[t++] = myScheme.digit3;
    toPrint[t++] = myScheme.digit4;
    toPrint[t++] = 'm';

    return t;
    }





void printSubMap(struct Map* myMap, uint8_t x, uint8_t y, uint8_t sizeX, uint8_t sizeY) {

    uint8_t i, j;


    char toPrint[sizeX * 10 + 2]; // *10 ??




    for (j = y; j < y + sizeY; j++) {
        uint8_t t = 0;

        for (i = x / 4; i < (x + sizeX) / 4 + 1; i++){

            uint8_t data = myMap->buffer[j * myMap->mySize + i];

            uint8_t d1, d2, d3, d4;
            d1 = (data & (3 << 0)) >> 0;
            d2 = (data & (3 << 2)) >> 2;
            d3 = (data & (3 << 4)) >> 4;
            d4 = (data & (3 << 6)) >> 6;

            uint8_t r = rand() % 3;

            t = tileScheme(&toPrint, t, d1); //ESC-codes tilføjes til toPrint, t opdateres
            toPrint[t++] = myMap->style[d1][r];
            if (d2 != d1)
            t = tileScheme(&toPrint, t, d2);
            toPrint[t++] = myMap->style[d2][r];
            if (d3 != d2)
            t = tileScheme(&toPrint, t, d3);
            toPrint[t++] = myMap->style[d3][r];
            if (d4 != d3)
            t = tileScheme(&toPrint, t, d4);
            toPrint[t++] = myMap->style[d4][r];
        }

        toPrint[t++] = '\n';
        toPrint[t++] = '\0';

        printf(toPrint);
    };
}



int main(void)
{
    srand(10321);

    uart_init(9600);
    homeCurser(); //Sæt curser til 0,0
    clearTermninal(); // Ryd terminal
    //struct Map myMap = {malloc(50 * 50 * sizeof(uint8_t)), 50, {{0xB0,0xB0,0xB0},  {0xDB,0xDB,0xDB}, {0xB3,0xDD,0xEF}, {0xF4, 0xF4, 0xF4}}, {1,2,3,4}};
    struct Map myMap = {malloc(50 * 50 * sizeof(uint8_t)), 50, {{0xDB,0xDB,0xDB},  {0xB2,0xB2,0xB2}, {0xF7,0xF7,0xF7}, {0xF4, 0xF4, 0xF4}}, {1,2,3,4}}; //R, W, G, L
    memset(myMap.buffer, 0x00, 50 * 50);
    buildMap(&myMap, 0, 0, 5, 10, 'G');
    buildMap(&myMap, 5, 0, 4, 10, 'R');
    buildMap(&myMap, 9, 0, 10, 10, 'G');
    buildMap(&myMap, 10, 1, 8, 8, 'W');


    while(1)
    {
        printSubMap(&myMap, 0,0, 20,10);
        gotoxy(0,0);
    };
}
