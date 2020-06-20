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
#include <string.h>

struct Map{

    uint8_t* buffer;
    char* renderBuffer;
    uint16_t mySize;
    char style[4][3];
    uint8_t colors[4];
};


struct Player{

    int16_t x;
    int16_t y;
    char look;
    uint8_t viewSize;

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


void printSubMap(struct Map* myMap, struct Player* player, uint8_t x, uint8_t y, uint8_t sizeX, uint8_t sizeY) {

    uint8_t i, j;
    uint32_t m = 0;
    uint8_t lastTile = -1; //ye, this is legal

    char toPrint[sizeX * 30 + 2]; // *10 ??


    for (j = y; j < y + sizeY; j++) {
        uint16_t t = 0;
        uint8_t xin = x;

        for (i = x ; i < x + sizeX; i++){

            uint8_t data = myMap->buffer[j * myMap->mySize + i / 4];

            uint8_t s = ((i % 3) * 2);
            uint8_t d = (data & (3 << s)) >> s;

            srand(i + j);

            uint8_t r = rand() % 3;

            //print the tile, optimazation, dont print new escape char if they are allready there
            if (d != lastTile){
                t = tileScheme(toPrint, t, d);
                lastTile = d;
            }

            //print tile or player
            if(j == player->y && xin == player->x){
                //print the player
                toPrint[t++] = player->look;
            }
            else{
                toPrint[t++] = myMap->style[d][r];
            }

            xin++;

        }

        toPrint[t++] = '\n';

        memcpy(&(myMap->renderBuffer[m]), toPrint, t);
        m += t;
    };

    myMap->renderBuffer[m + 1] = '\0';
}

volatile int a = 0;

static uint8_t mem[100*100];

int main(void)
{

    uart_init(115200);
    homeCurser(); //Sæt curser til 0,0
    clearTermninal(); // Ryd terminal

    struct Player player = {10, 10, 'P', 10};

    //struct Map myMap = {malloc(50 * 50 * sizeof(uint8_t)), 50, {{0xB0,0xB0,0xB0},  {0xDB,0xDB,0xDB}, {0xB3,0xDD,0xEF}, {0xF4, 0xF4, 0xF4}}, {1,2,3,4}};
    struct Map myMap = {mem, calloc(30 * player.viewSize * player.viewSize * sizeof(char), sizeof(uint8_t)), 50, {{0xB0,0xB0,0xB0},  {0xDB,0xDB,0xDB}, {0xB3,0xDD,0xEF}, {0xF4, 0xF4, 0xF4}}, {1,2,3,4}}; //R, W, G, L
    memset(myMap.buffer, 0x00, 50 * 50);
    buildMap(&myMap, 0, 0, 5, 10, 'G');
    buildMap(&myMap, 5, 0, 4, 10, 'R');
    buildMap(&myMap, 9, 0, 10, 10, 'G');
    buildMap(&myMap, 10, 1, 8, 8, 'W');


    RCC->AHBENR |= RCC_AHBPeriph_GPIOA; //Enabling clock for IO Port A
    RCC->AHBENR |= RCC_AHBPeriph_GPIOB;
    RCC->AHBENR |= RCC_AHBPeriph_GPIOC;
    RCC->AHBENR |= RCC_AHBPeriph_GPIOD;

    setPortMode(RIGHT_JOY_STICK, IN_MODE); //Porten RJS er forbundet til sættes til input-mode.
    setPortPuPd(RIGHT_JOY_STICK, NO_PULL); //Porten er forbundet til 1/0 i hardware, det er ikke nødvendigt med pull ?? Anders

    setPortMode(UP_JOY_STICK, IN_MODE);
    setPortPuPd(UP_JOY_STICK, NO_PULL);

    setPortMode(CENTER_JOY_STICK, IN_MODE);
    setPortPuPd(CENTER_JOY_STICK, NO_PULL);

    setPortMode(LEFT_JOY_STICK, IN_MODE);
    setPortPuPd(LEFT_JOY_STICK, NO_PULL);

    setPortMode(DOWN_JOY_STICK, IN_MODE);
    setPortPuPd(DOWN_JOY_STICK, NO_PULL);

    while(1)
    {

        uint8_t joy = readJoystick();

        if(joy & 0x1){
            //up
            player.y--;
        }
        if(joy & 0x2){
            //up
            player.y++;
        }
        if(joy & 0x4){
            //up
            player.x--;
        }
        if(joy & 0x8){
            //up
            player.x++;
        }

        player.x = MAX(player.x,  0);
        player.y = MAX(player.y,  0);
        player.x = MIN(player.x,  myMap.mySize - 1);
        player.y = MIN(player.y,  myMap.mySize - 1);

        //the position used as the players position (we max out at the bounderies of the map)
        uint16_t showX = MIN(myMap.mySize - 1 - player.viewSize, MAX(0, player.x - player.viewSize));
        uint16_t showY = MIN(myMap.mySize - 1 - player.viewSize, MAX(0, player.y - player.viewSize));

        //render position, where should we render the player
        uint16_t rPosX = MIN(myMap.mySize - 1, MAX(0, player.x - player.viewSize));
        uint16_t rPosY = MIN(myMap.mySize - 1, MAX(0, player.y - player.viewSize));

        printSubMap(&myMap, &player, showX, showY, player.viewSize * 2, player.viewSize * 2);
        gotoxy(0,0);
        printf(myMap.renderBuffer);

        int i = 0;
        for(i = 0; i < 10000; i++){
            a = i + i;
        }



    };
}
