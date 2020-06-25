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
#include "Radio.h"

#define DO_NOTHING 0
#define FRONTAL_ATTACK 1
#define REVERSE_ATTACK 2
#define TURN 3
#define HORN 4
#define FLEE 5



uint8_t* posXHPBOOZT;
uint8_t* posYHPBOOZT;
uint8_t* bOOZTuSED;



struct Map{

    uint8_t* buffer;

    uint16_t mySize;
    char style[4][3];
    uint8_t colors[4], posX, posY;

};

void calcAkse(int16_t pos, int16_t Size, int16_t bSize, int16_t b, int16_t* s, int16_t* z, uint8_t* enableBuild){

    if ((pos - (b + bSize)) < (Size / 2) || (b - pos) < (Size / 2)) {//afstanden mellem player og blok er mindre end et halvt vindue

        *enableBuild = 1; //Der skrives til array hvis et if-statement aktiveres

        if ((pos - b) <= (Size / 2) && (pos - b) > 0) { //Boks starter i 2. eller 3. kvadrant
            *s = (Size / 2) - (pos - b);
                if ((pos + (Size / 2) - b) >= bSize) { //Hvis resten af vinduet er l�ngere end boksen
                    *z = bSize;
                    } else {
                    *z = pos + (Size / 2) - b; //Hvis boksen g�r ud over vinduet
                    }
        }

        if (b >= pos) { // Boksen starter i 1. eller 4. kvadrant

            *s = (Size / 2) + b - pos;
                if ((Size / 2) - (b - pos) >= bSize) {//Hvis resten af vinduet er l�ngere end boksen
                    *z = bSize;
                } else {
                    *z = (Size / 2) - (b - pos); //Hvis boksen g�r ud over vinduet
                }

        } else if (b < (pos - (Size / 2))) { //Boksen starter udenfor vinduet
            *s = 0;
            *z = bSize - (pos - (Size / 2) - b);
        }
    }

    *s = MAX(0, MIN(Size, *s));
    *z = MAX(0, MIN(Size, *z));
}

struct Player{

    int16_t x;
    int16_t y;
    char look;
    uint8_t viewSize;

};

void buildMap(struct Map* myMap, uint16_t bx, uint16_t by, uint16_t sizeX, uint16_t sizeY, char style) {

    uint16_t i, j;
    uint8_t type, enableBuildX = 0, enableBuildY = 0;

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
    calcAkse(myMap->posX, myMap->mySize, sizeX, bx, &xs, &xz, &enableBuildX);

    calcAkse(myMap->posY, myMap->mySize, sizeY, by, &ys, &yz, &enableBuildY);

    if (enableBuildX && enableBuildY) { //Der skrives til buffer, hvis figuren på begge akser ligger inden for radius
        for (i = xs; i < xs + xz; i++) {
            for (j = ys; j < ys + yz; j++) {
                uint8_t shiftIndex = (i % 4) * 2;
                uint16_t index = i / 4;

                if(j > myMap->mySize || i > myMap->mySize)
                    continue;

                myMap->buffer[index /*% myMap->mySize*/ + j * myMap->mySize / 4 ] &= ~(0x3 << shiftIndex); //tile resettes //index % myMap->mySize er altid lig index ??
                myMap->buffer[index /*% myMap->mySize*/ + j * myMap->mySize / 4 ] |= (type << shiftIndex);
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
        myScheme.digit1 = '4';
        myScheme.digit2 = '7';
        myScheme.digit3 = '3';
        myScheme.digit4 = '0';
        myScheme.inv = 0;

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
        myScheme.digit1 = '3';
        myScheme.digit2 = '7';
        myScheme.digit3 = '4';
        myScheme.digit4 = '1';
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

    uint8_t i, j, t;


    char toPrint[sizeX * sizeY * 10 + 2]; // *10 ??

    for (j = 0; j < myMap->mySize; j++) {
        t = 0;
        uint8_t lastTile = 0;

        for (i = 0; i < myMap->mySize / 4; i++){

            uint8_t data = myMap->buffer[j * myMap->mySize / 4 + i];

            uint8_t d1, d2, d3, d4;
            d1 = (data & (3 << 0)) >> 0;
            d2 = (data & (3 << 2)) >> 2;
            d3 = (data & (3 << 4)) >> 4;
            d4 = (data & (3 << 6)) >> 6;

            uint16_t posX = myMap->posX + i * 4;
            uint16_t posY = myMap->posY + j;
            uint16_t b = (posX * 5) % 231 + posY % 51;
            srand(b);
            uint16_t r = rand() % 3;

            t = tileScheme(&toPrint, t, d1); //ESC-codes tilføjes til toPrint, t opdateres
            if(i == 4 && j == 16)
                toPrint[t++] = 0xb8;
            else{
                toPrint[t++] = myMap->style[d1][r];
            }

            posX++;
            b = (posX * 5) % 231 + posY % 51;
            srand(b);
            r = rand() % 3;

            if (d2 != d1)
            t = tileScheme(&toPrint, t, d2);
            toPrint[t++] = myMap->style[d2][r];

            posX++;
            b = (posX * 5) % 231 + posY % 51;
            srand(b);
            r = rand() % 3;

            if (d3 != d2)
            t = tileScheme(&toPrint, t, d3);
            toPrint[t++] = myMap->style[d3][r];

            posX++;
            b = (posX * 5) % 231 + posY % 51;
            srand(b);
            r = rand() % 3;

            if (d4 != d3)
                t = tileScheme(&toPrint, t, d4);
                toPrint[t++] = myMap->style[d4][r];
        }

        toPrint[t++] = '\n';
        toPrint[t++] = '\0';

        printf(toPrint);
    };
}

void builds(struct Map * myMap) {
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

    //Minor Roads
    buildMap(myMap, 6, 38, 44, 3, 'R'); // Fysikvej
    buildMap(myMap, 46, 19, 4, 19, 'R'); // FV2
    buildMap(myMap, 18, 93, 3, 29, 'R'); //Diplomvej
    buildMap(myMap, 21, 93, 9, 2, 'R'); // DPV2
    buildMap(myMap, 30, 82, 3, 13, 'R'); // DPV3
    buildMap(myMap, 49, 87, 3, 35, 'R'); // Centrifugevejint main(void)

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

   buildMap(myMap, 29, 12, 41, 6, 'W'); // B309
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
    buildMap(myMap, 8, 83, 20, 3, 'W'); // Science Park
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

    //sektor 3
    buildMap(myMap, 177, 7, 37, 5, 'W'); // Kraftvarmeværk
    buildMap(myMap, 211, 12, 4, 6, 'W'); // KV2
    buildMap(myMap, 177, 12, 9, 6, 'W'); // KV3
    buildMap(myMap, 197, 17, 6, 3, 'W'); // SIlo
    buildMap(myMap, 198, 16, 4, 5, 'W'); // Silo 2
    buildMap(myMap, 173, 49, 12, 23, 'W'); // Diamanten
    buildMap(myMap, 184, 42, 9, 14, 'W'); // D2
    buildMap(myMap, 184, 39, 2, 3, 'W'); // D3
    buildMap(myMap, 172, 86, 11, 33, 'W'); // B423
    buildMap(myMap, 205, 85, 11, 31, 'W'); // B427

    //Sektor 4
    buildMap(myMap, 225, 7, 5, 5, 'W'); //VKR1
    buildMap(myMap, 230, 7, 16, 3, 'W'); // VKR2
    buildMap(myMap, 242, 3, 4, 7, 'W'); // VKR3
    buildMap(myMap, 233, 17, 13, 3, 'W'); // VKR4

    buildMap(myMap, 226, 29, 5, 10, 'W'); //Netto
    buildMap(myMap, 235, 29, 5, 10, 'W'); //Kampsax
    buildMap(myMap, 231, 37, 5, 10, 'W');
    buildMap(myMap, 226, 45, 5, 10, 'W');
    buildMap(myMap, 231, 53, 5, 10, 'W');
    buildMap(myMap, 226, 61, 5, 10, 'W');
    buildMap(myMap, 231, 69, 5, 10, 'W');
    buildMap(myMap, 226, 78, 5, 10, 'W');
    buildMap(myMap, 235, 78, 5, 10, 'W');
    buildMap(myMap, 231, 86, 5, 10, 'W');
    buildMap(myMap, 226, 94, 5, 10, 'W');
    buildMap(myMap, 235, 94, 5, 10, 'W');

    buildMap(myMap, 242, 29, 5, 10, 'W');
    buildMap(myMap, 245, 37, 5, 10, 'W');
    buildMap(myMap, 242, 45, 5, 10, 'W');
    buildMap(myMap, 245, 53, 5, 10, 'W');
    buildMap(myMap, 242, 61, 5, 10, 'W');
    buildMap(myMap, 245, 69, 5, 10, 'W');
    buildMap(myMap, 242, 78, 5, 10, 'W');
    buildMap(myMap, 245, 86, 5, 10, 'W');
    buildMap(myMap, 242, 86, 5, 10, 'W');
    buildMap(myMap, 245, 94, 5, 10, 'W');

    buildMap(myMap, 242, 96, 6, 16, 'W');
    buildMap(myMap, 230, 112, 18, 5, 'W');
    buildMap(myMap, 225, 115, 6, 5, 'W');








    /*buildMap(myMap, 0, 0, 5, 10, 'G');
    buildMap(myMap, 5, 0, 4, 10, 'R');
    buildMap(myMap, 9, 0, 10, 10, 'G');
    buildMap(myMap, 10, 1, 8, 8, 'W'); */
}

void takeHeart(uint8_t* hp, uint8_t x, uint8_t y){

    *hp = 255;

    for(int i = 0; i < 100; i++){

        if(x == posXHPBOOZT[i] && y == posYHPBOOZT[i]){
            bOOZTuSED[i] = 1;
        }
    }

}

uint8_t motion(struct Map* myMap, char key, uint8_t* hp) {

    uint8_t m = myMap->mySize / 2 - 1;
    uint8_t ret = -1;

    //NÃ¦ste tile
    uint8_t indexN = 127 - (myMap->mySize / 8) + 1;
    uint8_t indexS = 127 + (myMap->mySize / 8) + (myMap->mySize / 4) + 1;
    uint8_t indexE = 127 + (myMap->mySize / 8) + 1;
    uint8_t indexW = 127 + (myMap->mySize / 8);
    uint8_t nextN = myMap->buffer[indexN] & 0x3; // next north
    uint8_t nextS = myMap->buffer[indexS] & 0x3; //next south
    uint8_t nextE = (myMap->buffer[indexE] & (0x3 << 2)) >> 2; //next east
    uint8_t nextW = (myMap->buffer[indexW] & (0x3 << 6)) >> 6; //next West

    //this tile


    if (key == 'w') {
        if(myMap->posY > m && nextN == 3){ //kÃ¸r nord
            myMap->posY--;
            ret = nextN;
            takeHeart(hp, myMap->posX, myMap->posY);
        }
        else if(myMap->posY > m && nextN != 1){
            myMap->posY--;
            ret = nextN;
        }
    } else if (key == 's') {

        if (myMap->posY < 127 - m && nextS == 3) { //kÃ¸r syd
            myMap->posY++;
            ret = nextS;
            takeHeart(hp, myMap->posX, myMap->posY);
        } else if(myMap->posY < 127 - m && nextS != 1){
            myMap->posY++;
            ret = nextS;
        }
    }
    else if (key == 'a') {
        if (myMap->posX > m && nextW == 3) { // kÃ¸r vest
            myMap->posX--;
            ret = nextE;
            takeHeart(hp, myMap->posX, myMap->posY);
        } else if ((myMap->posX > m && nextW != 1) ) {
            myMap->posX--;
            ret = nextE;
        }
    } else if (key == 'd') {
        if(myMap->posX < 255 - m && nextE == 3) { // kÃ¸r Ã¸st
            myMap->posX++;
            ret = nextW;
            takeHeart(hp, myMap->posX, myMap->posY);
        } else if (myMap->posX < 255 - m && nextE != 1) {
            myMap->posX++;
            ret = nextW;
        }
    }

    return ret;
}

void keyCommands(char * key, uint8_t * bEnable) {

    if (*key == 'b') {
        if (*bEnable == 1) {
        bossKey(&key);
        }
        blink(0);
        *key = '\0';
    }
}

void bossKey(char * key) {
    gotoxy(0,0);
    bgcolor(0);
    clearTermninal();
    blink(1);
    gotoxy(8,15);
    printf("Sending important");
    gotoxy(8,16);
    printf("business emails");
}

struct GlobalInfo {
    uint8_t isInBattle;
    uint8_t battingType;//0 = nobattle, 1 = standart lygtepel, 2 = wild, 3 = super....
    uint8_t gameOver;

};

struct Image {

    uint8_t* imageData;
    uint16_t width;
    uint16_t heigth;

};

//{"\e[40m1", "\e[40m2", "\e[40m3", "\e[40m4", "\e[40m5"}
static char charTransationTable[][10] = {"\e[40m ", "\e[47m ", "\e[107m ", "\e[103m ", "\e[41m ", "\e[46m ", "\e[100m "}; //Black , white, bright white, bright yellow, red, cyan, bright black

void printSubImage(struct Image* img,
                    uint8_t xStart, uint8_t yStart, uint8_t xEnd, uint8_t yEnd,
                    uint8_t xScreen, uint8_t yScreen){

    gotoxy(xScreen, yScreen);



        for(uint8_t y = yStart; y < yEnd; y++){

            gotoxy(xScreen, yScreen + y);
            for(uint8_t x = xStart; x < xEnd; x++){
                uint8_t charIndex = img->imageData[y * img->width + x];
                char* a = charTransationTable[charIndex];
                printf(a);
            }
        }

}


void fadeInEnemy(struct Image* img){

    for(uint8_t i = 0; i < img->heigth; i++){

       // printSubImage(img, 0, 0, img->width, i, 3, 3);
    }

}


void lcd_battle(uint8_t* buf, uint8_t* dir, uint8_t hoverPosition){


    if(*dir)
        lcd_write_string(buf, "   Reverse   ", 0, 0, hoverPosition == 0);
    else {
        lcd_write_string(buf, "   Frontal   ", 0, 0, hoverPosition == 0);
    }
    lcd_write_string(buf, "   3ptar?  ", 13, 0, hoverPosition == 1);
    lcd_write_string(buf, "   HOORN!   ", 0, 2,  hoverPosition == 2);
    lcd_write_string(buf, "   Flee..   ", 13, 2, hoverPosition == 3);

}

struct Fighter{
    uint8_t hp;
    char* name;
};

void drawHealth(struct Fighter* fighter, uint8_t x, uint8_t y) {
    uint8_t color;
    if (fighter->hp > 170) {
        color = 42;
    } else if (fighter->hp > 80)
    {
        color = 103;
    }
    else{
        color = 41;
    }

    printf("%c[%dm", 27, 40); //red
    window(y-2,x-2,2,17,'A',fighter->name);
    gotoxy(x,y);
    uint8_t boxes = fighter->hp >> 3 + 1; //divide by 16
    printf("%c[%dm", 27, color); //red
    for (uint8_t i = 0; i < boxes; i++) {
        printf(" ");
    }
    printf("%c[%dm", 27, 40); //black
    for (uint8_t i = 0; i < (16 - boxes); i++) {
        printf(" ");
    }

}


void placeHearts(struct Map * myMap) {

    for (uint8_t i = 0; i < 100; i++) {
        if(!bOOZTuSED[i])
            buildMap(myMap, posXHPBOOZT[i], posYHPBOOZT[i], 1, 1, 'H');
    }


}

//return 0 if do nothing, 1 to frontal attack, 2 to reverse attack, 3 to turn, 4 to horn, 5 to flee
uint8_t updateHoverPosition(uint8_t* currentHover, uint8_t* dir) {
    uint8_t val = readJoystick();
    uint8_t center = (val & 16) != 0;

    uint8_t ret = DO_NOTHING;

    if (*currentHover == 0) {
        if ((val & 0x8) != 0) { //right
            *currentHover = 1;
        } else if ((val & 0x2) != 0) { //down
            *currentHover = 2;
        }

        if(center){
            if(*dir)
                ret = REVERSE_ATTACK;
            else
                ret = FRONTAL_ATTACK;
        }
    }

    else if (*currentHover == 1) {
        if ((val & 0x4) != 0) { //left
            *currentHover = 0;
        } else if ((val & 0x2) != 0) { // down
            *currentHover = 3;
        }

        if(center){
            ret = TURN;
        }
    }

    else if (*currentHover == 2) {
        if ((val & 0x1) != 0) { //up
            *currentHover = 0;
        } else if ((val & 0x8) != 0) { //right
            *currentHover = 3;
        }


        if(center){
            ret = HORN;
        }
    }

     else if (*currentHover == 3) {
        if ((val & 0x1) != 0) { //up
            *currentHover = 1;
        } else if ((val & 0x4) != 0) { //left
            *currentHover = 2;

        }

        if(center){
            ret = FLEE;
        }
    }

    return ret;
}

void GameOver(uint8_t* score, struct GlobalInfo* info) {
    info->gameOver = 1;
    info->isInBattle = 0;
    clearTermninal();
    gotoxy(16,8);
    printf("GAME OVER\n               Your score was %d", *score);

}

void wait(uint32_t delay){
    uint32_t cnt = 0;
    uint32_t start = getTimer15Cnt();
    while (cnt < start + delay) {
        cnt = getTimer15Cnt();
    }
}

int main(void)
{
    struct GlobalInfo info = {0, 0, 0};

    uart_init(315200);
    homeCurser(); //Sæt curser til 0,0
    showCursor(0);
    clearTermninal(); // Ryd terminal
    //struct Map myMap = {malloc(50 * 50 * sizeof(uint8_t)), 50, {{0xB0,0xB0,0xB0},  {0xDB,0xDB,0xDB}, {0xB3,0xDD,0xEF}, {0xF4, 0xF4, 0xF4}}, {1,2,3,4}};
    struct Map myMap = {malloc(8 * 33 * sizeof(uint8_t)), 32, {{' ',' ',' '},  {0xB2,0xB2,0xB2}, {',',' ','"'}, {'+', '+', '+'}}, {1,2,3,4}}; //R, W, G, L

    posXHPBOOZT = calloc(100, sizeof(uint8_t));
    posYHPBOOZT = calloc(100, sizeof(uint8_t));
    bOOZTuSED = calloc(100, sizeof(uint8_t));

    for(int i = 0; i < 100; i++){
        posXHPBOOZT[i] = rand() % 255;
        posYHPBOOZT[i] = rand() % 127;
    }

    //memset(myMap.buffer, 0x00, 128 * 256);

    uint8_t dir = 0; //0 = front, 1 = back
    struct Image lamp ={0, 8, 24};
    struct Image van = {0,24,10};
    uint16_t score = 0;

    //setup joystick
    {
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
    }



    uint8_t lamparr[] = {
                        0,0,1,1,1,1,0,0,
                        1,1,1,1,1,1,1,1,
                        0,0,3,2,2,3,0,0,
                        0,0,3,2,2,3,0,0,
                        0,0,3,2,2,3,0,0,
                        0,0,3,3,3,3,0,0,
                        0,0,3,3,3,3,0,0,
                        0,0,0,1,1,0,0,0,
                        0,0,0,1,1,0,0,0,
                        0,0,0,1,1,0,0,0,
                        0,0,0,1,1,0,0,0,
                        0,0,0,1,1,0,0,0,
                        0,0,0,1,1,0,0,0,
                        0,0,0,1,1,0,0,0,
                        0,0,0,1,1,0,0,0,
                        0,0,0,1,1,0,0,0,
                        0,0,0,1,1,0,0,0,
                        0,0,0,1,1,0,0,0,
                        0,0,0,1,1,0,0,0,
                        0,0,0,1,1,0,0,0,
                        0,1,1,1,1,1,1,0,
                        0,1,1,1,1,1,1,0,
                        0,1,1,1,1,1,1,0,
                        0,1,1,1,1,1,1,0
                    };

    uint8_t vanarr[] = {
                        0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,
                        0,2,2,4,4,4,2,4,4,4,2,4,2,4,2,2,2,5,5,5,0,0,0,0,
                        0,2,2,4,2,4,2,2,4,2,2,4,2,4,2,2,2,5,5,5,5,0,0,0,
                        0,2,2,4,2,4,2,2,4,2,2,4,2,4,2,2,2,5,5,5,5,5,0,0,
                        0,2,2,4,4,4,2,2,4,2,2,4,4,4,2,2,2,2,2,2,2,2,2,0,
                        0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,
                        2,2,2,2,6,6,6,6,2,2,2,2,2,2,2,2,6,6,6,6,2,2,2,3,
                        2,2,2,6,6,6,6,6,6,2,2,2,2,2,2,6,6,6,6,6,6,2,2,3,
                        0,2,2,6,6,6,6,6,6,2,2,2,2,2,2,6,6,6,6,6,6,2,2,0,
                        0,0,0,0,6,6,6,6,0,0,0,0,0,0,0,0,6,6,6,6,0,0,0,0
                        };

    lamp.imageData  = &lamparr;
    van.imageData  = &vanarr;
    myMap.posX = 16;
    myMap.posY = 16;
    char key;
    char nextKey;
    uint8_t bossEnable = 0;
    char radioToPrint[256];
    uint8_t buf[512];
    lcd_graphics_buffer(buf, 512);
    uint8_t hoverPosition = 0;
    uint8_t lastReadJoy = 0;
    uint8_t turn = 0;
    setupRadio(radioToPrint);

    struct Fighter vanFighter = {255, "Van "};
    struct Fighter currentEnemy = {0, ""};


    // TImer ---------------------------------
   // initTimer3();


    //-----------------------------------------


    //builds(&myMap, posX, posY,);

    while(1)
    {
        inverse(0);
        uint8_t encounter = 0;
        nextKey = uart_get_char();
        uart_clear();
        if (nextKey != '\0') {
            if (nextKey == 'b')
                bossEnable = !bossEnable;

            key = nextKey;
        }

        if(!info.isInBattle && !info.gameOver && !bossEnable){
            dir = 0;
            builds(&myMap);
            placeHearts(&myMap);

            updateRadio(radioToPrint, buf);


            printSubMap(&myMap, 0,0, 32,32);
            encounter = motion(&myMap, key, &(vanFighter.hp));

            if(encounter == 3){
                //enter battle
            }
            else if (encounter == 2) {
                //srand();
                uint8_t foundWild = (rand() % 20 == 15); //10 % change to encounter wild pokemon
                if(foundWild){
                    info.isInBattle = 1;
                    bgcolor(0);
                    inverse(0);
                    currentEnemy.hp = 255;
                    currentEnemy.name = "lamppost";
                    clearTermninal();
                    lcd_graphics_buffer(buf, 512);
                    //fadeInEnemy(&lamparr);
                }
            }

        }
        else if(bossEnable){

            updateRadio(radioToPrint, buf);
            keyCommands(&key, &bossEnable);

        }
        else if (info.gameOver != 1) {
            battleMusic();
            gotoxy(0,0);
            printSubImage(&lamp, 0, 0, lamp.width, lamp.heigth, 25, 3);
            printSubImage(&van, 0, 0, van.width, van.heigth, 2, 30);
            drawHealth(&currentEnemy, 6, 10);
            drawHealth(&vanFighter, 30, 35);
            uint8_t action = DO_NOTHING;

            lcd_battle(&buf, &dir, hoverPosition);
            lcd_push_buffer(&buf);

      /*----*/  if (turn == 0) {

                gotoxy(15,28);
                clreol();


                if(readJoystick() != lastReadJoy){
                    action = updateHoverPosition(&hoverPosition, &dir);
                    lastReadJoy = readJoystick();
                }

                if(action == FRONTAL_ATTACK){
                    uint8_t dmg = 20 + rand() % 10;
                    turn = !turn;

                    gotoxy(10,28);
                    printf("You crashed forwards into wild lamppost!");
                    gotoxy(10,29);
                    printf("Lamppost took %d damage!", dmg);
                    wait(150);


                    if(dmg > currentEnemy.hp){


                        info.isInBattle = 0;
                        clearTermninal();
                        gotoxy(8,16);
                        printf("Wild lamppost defeated!\n          You gained a point!");
                        lcd_graphics_buffer(buf, 512);
                        lcd_push_buffer(buf);
                        score++;
                        wait(150);
                    }

                    currentEnemy.hp -= dmg;

                }
                else if(action == REVERSE_ATTACK){
                    uint8_t dmg = 30 + rand() % 20;
                    turn = !turn;
                    ;
                    gotoxy(10,28);
                    printf("You reversed into wild lamppost!");
                    gotoxy(10,29);
                    printf("Lamppost took %d damage!", dmg);
                    wait(150);

                    if(dmg > currentEnemy.hp){


                        info.isInBattle = 0;
                        clearTermninal();
                        gotoxy(8,16);
                        printf("Wild lamppost defeated!\n          You gained a point!");
                        lcd_graphics_buffer(buf, 512);
                        lcd_push_buffer(buf);
                        score++;
                        wait(150);
                    }

                    currentEnemy.hp -= dmg;


                }

                else if (action == HORN) {
                    turn = !turn;
                    gotoxy(10,28);
                    printf("You used your horn at wild lamppost!");
                    gotoxy(10,29);
                    printf("Attack was inefficient!");
                     wait(150);

                }
                else if(action == TURN) {
                    dir = !dir;
                    turn = !turn;
                    gotoxy(10,28);
                    printf("You performed a three-point turn-around!");
                    wait(150);
                }
                else if(action == FLEE){

                    info.isInBattle = 0;
                    lcd_graphics_buffer(buf, 512);
                    lcd_push_buffer(buf);
                    clearTermninal();
                    if (vanFighter.hp > 20) {
                        vanFighter.hp -= 20;
                    } else {
                        GameOver(&score, &info);
                    }
                    gotoxy(10,28);
                    printf("You escaped!");
                    gotoxy(10,29);
                    printf("You took %d damage!", 20);
                    wait(150);
                }

            gotoxy(10,28);
            clreol();
            gotoxy(10,29);
            clreol();


            }
            if (turn == 1 && info.isInBattle) {
                turn = !turn;
                gotoxy(10,28);
                printf("Wild lamppost used 'blind'!");
                uint8_t dmg = rand() % 20 + score * 2;
                gotoxy(10,29);
                printf("You took %d damage!", dmg);
                if (vanFighter.hp > dmg) {
                    vanFighter.hp -= dmg;
                } else {
                    GameOver(&score, &info);
                }
                wait(150);
            }

            gotoxy(10,28);
            clreol();
            gotoxy(10,29);
            clreol();
        }

        key = '\0';
        gotoxy(0,0);

    }
}
