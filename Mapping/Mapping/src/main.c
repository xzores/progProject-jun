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
    uint8_t colors[4], posX, posY;

};

void calcAkse(int16_t pos, int16_t Size, int16_t bSize, int16_t b, int16_t* s, int16_t* z, uint8_t* enableBuild){

    if ((pos - (b + bSize)) < (Size / 2) || (b - pos) < (Size / 2)) {//afstanden mellem player og blok er mindre end et halvt vindue

        *enableBuild = 1; //Der skrives til array hvis et if-statement aktiveres

        if ((pos - b) < (Size / 2)) { //Boks starter i 2. eller 3. kvadrant
            *s = (Size / 2) - pos - b;
                if ((pos + (Size / 2) - b) > bSize) { //Hvis resten af vinduet er l�ngere end boksen
                    *z = bSize;
                    } else {
                    *z = pos + (Size / 2) - b; //Hvis boksen g�r ud over vinduet
                    }

        }

        if (b > pos) { // Boksen starter i 1. eller 4. kvadrant

            *s = (Size / 2) + b - pos;
                if ((Size / 2) - (b - pos) > bSize) {//Hvis resten af vinduet er l�ngere end boksen
                    *z = bSize;
                } else {
                    *z = (Size / 2) - (b - pos); //Hvis boksen g�r ud over vinduet
                }

        } else if (b < (pos - (Size / 2))) { //Boksen starter udenfor vinduet
            *s = 0;
            *z = bSize - (pos - (Size / 2) - b);
        }
    }

    *s = MAX(0, MIN(Size - 1, *s));
    *z = MAX(0, MIN(Size - 1, *z));
}

struct Player{

    int16_t x;
    int16_t y;
    char look;
    uint8_t viewSize;

};

void buildMap(struct Map* myMap, uint16_t bx, uint16_t by, uint16_t sizeX, uint16_t sizeY, char style) {

    uint16_t i, j;
    uint8_t type, enableBuild = 0;

    if (style == 'R') {
        type = 0;
    } else if(style == 'W') {
        type = 1;
    } else if (style == 'G') {
        type = 2;
    } else {
        type = 3;
    }

    int16_t xs, ys, xz, yz;

    //X-akse
    calcAkse(myMap->posX, myMap->mySize, sizeX, bx, &xs, &xz, &enableBuild);

    calcAkse(myMap->posY, myMap->mySize, sizeY, by, &ys, &yz, &enableBuild);

    if (enableBuild) {
        for (i = xs; i < xs + xz; i++) {
            for (j = ys; j < ys + yz; j++) {
                uint8_t shiftIndex = (i % 4) * 2;
                uint16_t index = i / 4;
                myMap->buffer[index /*% myMap->mySize*/ + j * myMap->mySize] &= ~(0x3 << shiftIndex); //tile resettes //index % myMap->mySize er altid lig index ??
                myMap->buffer[index /*% myMap->mySize*/ + j * myMap->mySize] |= (type << shiftIndex);
            }
        }
    }

}



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
     } else if (style == 3) { //Lygtep�l, FG 93, BG 43, INv 0ff
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


builds(struct Map * myMap) {
    //Gr�s-l�rred
    buildMap(myMap, 0, 0, 255, 127, 'G');

    //Major Roads
    buildMap(myMap, 0, 0, 255, 4, 'R'); // Anker Engelundsvej
    buildMap(myMap, 0, 4, 6, 118, 'R'); // Lundtoftevej
    buildMap(myMap, 0, 123, 255, 4, 'R'); // Akademivej
    buildMap(myMap, 252, 4, 4, 121, 'R'); // Lundtofteg�rdvej
    buildMap(myMap, 150, 4, 4, 120, 'R'); // Asmussens All�
    buildMap(myMap, 165, 4, 4, 120, 'R'); //Niels Koppels All�
    buildMap(myMap, 219, 4, 4, 120, 'R'); // Kollegiebakken
    buildMap(myMap, 95, 64, 55, 3, 'R'); // Elektrovej
    buildMap(myMap, 6, 79, 100, 3, 'R'); // ELV2
    buildMap(myMap, 21, 66, 3, 13, 'R'); // ELV3
    buildMap(myMap, 21, 64, 34, 3, 'R'); // ELV4
/*
    //Minor Roads
    buildMap(myMap, 6, 38, 44, 3, 'R'); // Fysikvej
    buildMap(myMap, 46, 19, 4, 19, 'R'); // FV2
    buildMap(myMap, 18, 93, 3, 29, 'R'); //Diplomvej
    buildMap(myMap, 21, 93, 9, 2, 'R'); // DPV2
    buildMap(myMap, 30, 82, 3, 13, 'R'); // DPV3
    buildMap(myMap, 49, 87, 3, 35, 'R'); // Centrifugevej
    buildMap(myMap, 64, 98, 4, 24, 'R'); // Akustikvej
    buildMap(myMap, 200, 42, 11, 36, 'R'); // Energivej
    buildMap(myMap, 205, 42, 14, 34, 'R'); // EV2
    buildMap(myMap, 188, 58, 12, 15, 'R'); // EV-P
    buildMap(myMap, 226, 27, 12, 2, 'R'); // Nettovej
    buildMap(myMap, 238, 28, 3, 1, 'R'); // KXV2
    buildMap(myMap, 240, 29, 1, 11, 'R'); // KXV3
    buildMap(myMap, 238, 40, 3, 1, 'R'); // KXV4
    buildMap(myMap, 237, 40, 1, 16, 'R'); // KXV5
    buildMap(myMap, 237, 56, 3, 1, 'R'); // KVX6
    buildMap(myMap, 240, 56, 1 , 36, 'R'); // KXV7
    buildMap(myMap, 223, 92, 2, 29,'R'); // KXV1
    buildMap(myMap, 223, 97, 17, 2, 'R'); //KXV8



    // Torve og parkeringspladser
    buildMap(myMap, 43, 89, 18, 33, 'R'); // �rsted plads
    buildMap(myMap, 95, 29, 30, 22, 'R'); // Matematiktorvet
    buildMap(myMap, 125, 29, 7, 1, 'R'); // Mtorv2
    buildMap(myMap, 132, 27, 5, 3, 'R'); //Mtorv2
    buildMap(myMap, 223, 27, 3, 8, 'R'); // NETTOP
    buildMap(myMap, 229, 99, 11, 11, 'R'); //KX-P



    //Stier
    buildMap(myMap, 95, 51, 2, 13, 'R'); // sti 1
    buildMap(myMap, 47, 66, 1, 13, 'R'); // sti 5
    buildMap(myMap, 106, 81, 44, 1, 'R'); // Sti 6
    buildMap(myMap, 154,68, 11, 2, 'R'); // Sti 2
    buildMap(myMap, 154,98, 11, 2, 'R'); // Sti 3
    buildMap(myMap, 169,27, 50, 1, 'R'); // Sti 4

    //Bygninger/Walls Sektor 1m->
   */ buildMap(myMap, 29, 12, 41, 6, 'W'); // B309
    buildMap(myMap, 78, 9, 68, 14, 'W'); //B303
    buildMap(myMap, 78, 22, 11, 11, 'W'); //B306
    buildMap(myMap, 126, 31, 22, 6, 'W'); //B304
    buildMap(myMap, 141, 37, 7, 8, 'W'); //B0305
    buildMap(myMap, 127, 45, 21, 6, 'W'); //B321
    buildMap(myMap, 99, 52, 26, 9, 'W'); //B324
    buildMap(myMap, 126, 56, 21, 6, 'W'); //B322
    buildMap(myMap, 54, 37, 36, 12, 'W'); //B308
    buildMap(myMap, 65, 49, 3, 7, 'W'); //B3252
    buildMap(myMap, 48, 56, 45, 6, 'W'); //B325
    buildMap(myMap, 59, 62, 2, 5, 'W'); // SPace 2
    buildMap(myMap, 49, 67, 19, 6, 'W'); // DTU Space

    //Sektor 2
/* buildMap(myMap, 8, 83, 20, 3, 'W'); // Science Park
    buildMap(myMap, 8, 88, 8, 2, 'W'); // SP2
    buildMap(myMap, 8, 91, 8, 4, 'W'); // SP3
    buildMap(myMap, 17, 86, 5, 6, 'W'); // SP4
    buildMap(myMap, 7, 108, 6, 13, 'W'); // William Demant
    buildMap(myMap, 10, 97, 7, 17, 'W'); // WD2
    buildMap(myMap, 22, 110, 10, 10, 'W'); // Skylab
    buildMap(myMap, 32, 110, 5, 2, 'W'); // Sky 2
    buildMap(myMap, 22, 100, 10, 4, 'W'); // SKy 3
    buildMap(myMap, 37, 85, 7, 20, 'W'); // Sky 4
    buildMap(myMap, 56, 115, 4, 6, 'W'); // B1
    buildMap(myMap, 53, 111, 10, 4, 'W'); // B2
    buildMap(myMap, 59, 98, 1, 13, 'W'); // B3
    buildMap(myMap, 53, 103, 10, 3, 'W'); // B4
    buildMap(myMap, 56, 100, 3, 3, 'W'); // B5
    buildMap(myMap, 71, 83, 77, 5, 'W'); // B341
    buildMap(myMap, 78, 89, 14, 8, 'W'); // Hegnet
    buildMap(myMap, 81, 88, 8, 1, 'W'); // Hegnet 2
    buildMap(myMap, 118, 91, 30, 10, 'W'); // Fotonik1
    buildMap(myMap, 116, 88, 2, 14, 'W'); // F2
    buildMap(myMap, 116, 102, 32, 8, 'W'); // F3
    buildMap(myMap, 116, 111, 32, 10, 'W'); //
*/










    /*buildMap(myMap, 0, 0, 5, 10, 'G');
    buildMap(myMap, 5, 0, 4, 10, 'R');
    buildMap(myMap, 9, 0, 10, 10, 'G');
    buildMap(myMap, 10, 1, 8, 8, 'W'); */
}


static uint8_t mem[100*100];

int main(void)
{

    uart_init(115200);
    homeCurser(); //S�t curser til 0,0
    clearTermninal(); // Ryd terminal

    struct Player player = {10, 10, 'P', 10};

    //struct Map myMap = {malloc(50 * 50 * sizeof(uint8_t)), 50, {{0xB0,0xB0,0xB0},  {0xDB,0xDB,0xDB}, {0xB3,0xDD,0xEF}, {0xF4, 0xF4, 0xF4}}, {1,2,3,4}};
    struct Map myMap = {malloc(64 * 64 * sizeof(uint8_t)), 64, {{0xDB,0xDB,0xDB},  {0xB2,0xB2,0xB2}, {0xF7,0xF7,0xF7}, {0xF4, 0xF4, 0xF4}}, {1,2,3,4}}; //R, W, G, L

    //memset(myMap.buffer, 0x00, 128 * 256);


    myMap.posX = 33;
    myMap.posY = 33;







    //builds(&myMap, posX, posY,);



    RCC->AHBENR |= RCC_AHBPeriph_GPIOA; //Enabling clock for IO Port A
    RCC->AHBENR |= RCC_AHBPeriph_GPIOB;
    RCC->AHBENR |= RCC_AHBPeriph_GPIOC;
    RCC->AHBENR |= RCC_AHBPeriph_GPIOD;

    setPortMode(RIGHT_JOY_STICK, IN_MODE); //Porten RJS er forbundet til s�ttes til input-mode.
    setPortPuPd(RIGHT_JOY_STICK, NO_PULL); //Porten er forbundet til 1/0 i hardware, det er ikke n�dvendigt med pull ?? Anders

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

        printSubMap(&myMap, &player, showX, showY, player.viewSize * 2, player.viewSize * 2);
        gotoxy(0,0);
        printf(myMap.renderBuffer);


        gotoxy(0,0);
        myMap.posX++;
        myMap.posY++;
    };
}
