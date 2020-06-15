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


void printSubMap(struct Map* myMap, uint8_t x, uint8_t y, uint8_t sizeX, uint8_t sizeY) {

    uint8_t i, j;


    char toPrint[sizeX * 10 + 2]; // *10 ??
    uint8_t t = 0;

            toPrint[t++] = 0x1B; // ESC
            toPrint[t++] = 0x5B; // [
            toPrint[t++] = 41;
            toPrint[t++] = 'm';

            toPrint[t++] = 0x1B; // ESC
            toPrint[t++] = 0x5B; // [
            toPrint[t++] = 34;
            toPrint[t++] = 'm';

            toPrint[t++] = 0x1B; // ESC
            toPrint[t++] = 0x5B; // [
            toPrint[t++] = 7;
            toPrint[t++] = 'm';

    for (j = y; j < y + sizeY; j++) {

        t = 0;

        for (i = x / 4; i < (x + sizeX) / 4 + 1; i++){

            uint8_t data = myMap->buffer[j * myMap->mySize + i];

            uint8_t d1, d2, d3, d4;
            d1 = (data & (3 << 0)) >> 0;
            d2 = (data & (3 << 2)) >> 2;
            d3 = (data & (3 << 4)) >> 4;
            d4 = (data & (3 << 6)) >> 6;

            uint8_t r = rand() % 3;

            /*printf("%c[%dm", 27, 42);
            printf("%c[%dm", 7, 27);*/



            //toPrint[t++] = myMap->colors[d1] + 40;*/
            //toPrint[t++] = ("%c[%dm", 27, 41);

            toPrint[t++] = myMap->style[d1][r];
            toPrint[t++] = myMap->style[d2][r];
            toPrint[t++] = myMap->style[d3][r];
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
    struct Map myMap = {malloc(50 * 50 * sizeof(uint8_t)), 50, {{0xDB,0xDB,0xDB},  {0xB2,0xB2,0xB2}, {0xF7,0xFB,0xEF}, {0xF4, 0xF4, 0xF4}}, {1,2,3,4}}; //R, W, G, L
    memset(myMap.buffer, 0x00, 50 * 50);
    buildMap(&myMap, 0, 0, 5, 10, 'G');
    buildMap(&myMap, 5, 0, 4, 10, 'R');
    buildMap(&myMap, 9, 0, 10, 10, 'G');
    buildMap(&myMap, 10, 1, 8, 8, 'W');
    printSubMap(&myMap, 0,0, 20,20);

    while(1)
    {

    };
}
