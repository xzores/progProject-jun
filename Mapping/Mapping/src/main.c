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

//this part was made by jakob
//used to store data about health packs
uint8_t* posXHPBOOZT;
uint8_t* posYHPBOOZT;
uint8_t* bOOZTuSED;

//this part was made by anders and jakob
//map structure used for stong info about the player position and map tile data
struct Map{

    uint8_t* buffer;

    uint16_t mySize;
    char style[4][3];
    uint8_t colors[4], posX, posY;

};

//this part was made by anders and jakob
//helper function, used to check if what tiles should be printet to the tile buffer
void calcAkse(int16_t pos, int16_t Size, int16_t bSize, int16_t b, int16_t* s, int16_t* z, uint8_t* enableBuild){

    if ((pos - (b + bSize)) < (Size / 2) || (b - pos) < (Size / 2)) //distance from player to block is less than half a window


        *enableBuild = 1; //Must be one for block to be initialized in window

        if ((pos - b) <= (Size / 2) && (pos - b) > 0) { //Block is in 2nd or 3rd quadrant of window
            *s = (Size / 2) - (pos - b);
                if ((pos + (Size / 2) - b) >= bSize) { //remaining window is longer than the block
                    *z = bSize;
                    } else {
                    *z = pos + (Size / 2) - b; //Block is longer than remaining window
                    }
        }

        if (b >= pos) { //Block is in 1st or 4th quadrant of window

            *s = (Size / 2) + b - pos;
                if ((Size / 2) - (b - pos) >= bSize) {//remaining window is longer than the block
                    *z = bSize;
                } else {
                    *z = (Size / 2) - (b - pos); //Block is longer than remaining window
                }

        } else if (b < (pos - (Size / 2))) { //Block startpoint is outside of the window
            *s = 0;
            *z = bSize - (pos - (Size / 2) - b);
        }
    }

    *s = MAX(0, MIN(Size, *s));
    *z = MAX(0, MIN(Size, *z));
}

//this part was made by anders
//build the squre to the map
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

    if (enableBuildX && enableBuildY) { //Buffer gets data, if block on both axis is inside of radius to player

        for (i = xs; i < xs + xz; i++) {
            for (j = ys; j < ys + yz; j++) {
                uint8_t shiftIndex = (i % 4) * 2;
                uint16_t index = i / 4;

                if(j > myMap->mySize || i > myMap->mySize)
                    continue;

                myMap->buffer[index + j * myMap->mySize / 4 ] &= ~(0x3 << shiftIndex); //tile resettes
                myMap->buffer[index + j * myMap->mySize / 4 ] |= (type << shiftIndex);
            }
        }
    }

}

//this part was made by anders
//helper strcut used in tileScheme
struct visual{
    uint8_t digit1, digit2, digit3, digit4, inv;
} default_visual = {'3','7','4','0',0}; //ESC[37m ESC[40m, reset to white foreground, black background, inv off

//this part was made by anders
//helper function used to print the color of a tile
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
     } else if (style == 3) { //HP, FG 93, BG 43, INv 0ff
        myScheme.digit1 = '3';
        myScheme.digit2 = '7';
        myScheme.digit3 = '4';
        myScheme.digit4 = '1';
     }

     //Invers printed to array
     toPrint[t++] = 0x1B; // ESC
     toPrint[t++] = 0x5B; // [
     if (myScheme.inv == 0) {
         toPrint[t++] = '2'; } //7 or 27, on eller off
     toPrint[t++] = '7'; //7 for inverse, 27 for not-inverse
     toPrint[t++] = 'm';


    //Foreground and Background
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

//this part was made by jakob and anders
//prints a part of the map (aka the viewport) to the putty terminal
void printSubMap(struct Map* myMap, uint8_t x, uint8_t y, uint8_t sizeX, uint8_t sizeY) {

    uint8_t i, j, t;


    char toPrint[sizeX * sizeY * 10 + 2]; //Array to be filled with data, then printet to Putty

    for (j = 0; j < myMap->mySize; j++) {
        t = 0;

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

            t = tileScheme(&toPrint, t, d1); //ESC-codes added to toPrint, t is updated
            if(i == 4 && j == 16)
                toPrint[t++] = 0xb8;
            else{
                toPrint[t++] = myMap->style[d1][r];
            }

            posX++;
            b = (posX * 5) % 231 + posY % 51;
            srand(b);
            r = rand() % 3; //random printing one of three characters, to make variations in Grass-structure

            if (d2 != d1) //if previous tile is the same as this, for example Wall and Wall
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

//this part was made by anders
//this stores the campus map as a list of sqaures, these are build each frame.
void builds(struct Map * myMap) {
    //Grass-canvas
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



    // Squares and parking-lots
    buildMap(myMap, 43, 89, 18, 33, 'R'); // �rsted plads
    buildMap(myMap, 95, 29, 30, 22, 'R'); // Matematiktorvet
    buildMap(myMap, 125, 29, 7, 1, 'R'); // Mtorv2
    buildMap(myMap, 132, 27, 5, 3, 'R'); //Mtorv2
    buildMap(myMap, 223, 27, 3, 8, 'R'); // NETTOP
    buildMap(myMap, 229, 99, 11, 11, 'R'); //KX-P



    //small roads
    buildMap(myMap, 95, 51, 2, 13, 'R'); // sti 1
    buildMap(myMap, 47, 66, 1, 13, 'R'); // sti 5
    buildMap(myMap, 106, 81, 44, 1, 'R'); // Sti 6
    buildMap(myMap, 154,68, 11, 2, 'R'); // Sti 2
    buildMap(myMap, 154,98, 11, 2, 'R'); // Sti 3
    buildMap(myMap, 169,27, 50, 1, 'R'); // Sti 4

    //Buildings/Walls Sector 1

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

    //Sector 2
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

    //sector 3
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

    //Sector 4
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

}

//this part was made by jakob
void takeHeart(uint8_t* hp, uint8_t x, uint8_t y){

    //heals the player
    *hp = 255;

    //remove the health pack from the map (we need to check all the hp and then let the used bit high)
    for(int i = 0; i < 100; i++){

        if(x == posXHPBOOZT[i] && y == posYHPBOOZT[i]){
            bOOZTuSED[i] = 1;
        }
    }

}

//this part was made by anders
//this function takes the map and the pressed key and the vans(player) hp as a pointer
uint8_t motion(struct Map* myMap, char key, uint8_t* hp) {

    uint8_t m = myMap->mySize / 2 - 1;
    uint8_t ret = -1;

    //Next tile that we will land on is calculated
    uint8_t indexN = 127 - (myMap->mySize / 8) + 1;
    uint8_t indexS = 127 + (myMap->mySize / 8) + (myMap->mySize / 4) + 1;
    uint8_t indexE = 127 + (myMap->mySize / 8) + 1;
    uint8_t indexW = 127 + (myMap->mySize / 8);
    uint8_t nextN = myMap->buffer[indexN] & 0x3; // next north
    uint8_t nextS = myMap->buffer[indexS] & 0x3; //next south
    uint8_t nextE = (myMap->buffer[indexE] & (0x3 << 2)) >> 2; //next east
    uint8_t nextW = (myMap->buffer[indexW] & (0x3 << 6)) >> 6; //next West

    //check what key is pressed, if we land on a health pack heal the player, if we hit a wall dont move
    if (key == 'w') {
        if(myMap->posY > m && nextN == 3){ //nord
            myMap->posY--;
            ret = nextN;
            takeHeart(hp, myMap->posX, myMap->posY);
        }
        else if(myMap->posY > m && nextN != 1){
            myMap->posY--;
            ret = nextN;
        }
    } else if (key == 's') {

        if (myMap->posY < 127 - m && nextS == 3) { //syd
            myMap->posY++;
            ret = nextS;
            takeHeart(hp, myMap->posX, myMap->posY);
        } else if(myMap->posY < 127 - m && nextS != 1){
            myMap->posY++;
            ret = nextS;
        }
    }
    else if (key == 'a') {
        if (myMap->posX > m && nextW == 3) { //vest
            myMap->posX--;
            ret = nextE;
            takeHeart(hp, myMap->posX, myMap->posY);
        } else if ((myMap->posX > m && nextW != 1) ) {
            myMap->posX--;
            ret = nextE;
        }
    } else if (key == 'd') {
        if(myMap->posX < 255 - m && nextE == 3) { //east
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

// this part was made by anders
//blinks "sending important emils" on the screen
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
//Prints help-menu
void helpKey(char * key) {
    gotoxy(0,0);
    bgcolor(0);
    clearTermninal();
    gotoxy(3,3);
    window(3,3,26,26,'B',"Helpmenu "); //Window containing text

    gotoxy(6,6);
    printf("Move player with "); //Textstatements

    gotoxy(6,7);
    printf("W-A-S-D");

    gotoxy(6,9);
    printf("Score points by defea-");

    gotoxy(6,10);
    printf("ting wild lampposts!");

    gotoxy(6,11);
    printf("You'll find them in the");

    gotoxy(6,12);
    printf("grass! When in battle,");

    gotoxy(6,13);
    printf("use joystick to attack");

    gotoxy(6,14);
    printf("lamppost!");

    gotoxy(6,16);
    printf("The more lamppost you");

    gotoxy(6,17);
    printf("defeated, the more dam-");
    gotoxy(6,18);
    printf("age they'll do to");

    gotoxy(6,19);
    printf("your van!");

    gotoxy(6,21);
    printf("Make sure you get the ");

    gotoxy(6,22);
    printf("Powerups spread out in");

    gotoxy(6,23);
    printf("the map, to revitalise");

    gotoxy(6,24);
    printf("your HP!");

    gotoxy(6,26);
    printf("Remember to have some");

    gotoxy(6,27);
    printf("fun with the radio!");

    gotoxy(6,29);
    printf("Press 'h' to continue");
}

// this part was made by anders
//checks if we pressed the 'b' key
void keyCommands(char * key, uint8_t * bEnable, uint8_t * hEnable) {

    if (*key == 'b') {
        if (*bEnable == 1) {
        bossKey(&key);
        }
        blink(0); //disable blink when done with Bosskey
        *key = '\0';
    }

    if (*key == 'h') {
        if (*hEnable == 1) {
        helpKey(&key);
        }

        *key = '\0';
    }
}

// this part was made by anders and jakob
//stores global information
struct GlobalInfo {
    uint8_t isInBattle;
    uint8_t battingType;//0 = nobattle, 1 = standart lygtepel, 2 = wild, 3 = super....
    uint8_t gameOver;

};

// this part was made by anders and jakob
//the struct for string an image (uses the charTransationTable variable for colors)
struct Image {

    uint8_t* imageData;
    uint16_t width;
    uint16_t heigth;

};

// this part was made by anders and jakob
//he we use a global variable, because it makes it easier to index, and we should not have a problem since it is static
static char charTransationTable[][10] = {"\e[40m ", "\e[47m ", "\e[107m ", "\e[103m ", "\e[41m ", "\e[46m ", "\e[100m "}; //Black , white, bright white, bright yellow, red, cyan, bright black

// this part was made by anders and jakob
//prints a
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

//made by jakob
//displays the options in battle on the LCD.
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

//made by anders and jakob
struct Fighter{
    uint8_t hp;
    char* name;
};

//made by anders and Jakob
//draws the health of lamppost or the van
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
    uint8_t boxes = (fighter->hp >> 4) + 1; //divide by 16
    printf("%c[%dm", 27, color); //red
    for (uint8_t i = 0; i < boxes - 1; i++) {
        printf(" ");
    }
    printf("%c[%dm", 27, 40); //black
    for (uint8_t i = 0; i < (16 - boxes); i++) {
        printf(" ");
    }

}

//made by jakob
//places the health packs on the map, but only if they have not been taken
void placeHearts(struct Map * myMap) {

    for (uint8_t i = 0; i < 100; i++) {
        if(!bOOZTuSED[i])
            buildMap(myMap, posXHPBOOZT[i], posYHPBOOZT[i], 1, 1, 'H');
    }


}

//made by jakob and anders
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

//made by anders
//cleas terminal and write game over
void GameOver(uint16_t* score, struct GlobalInfo* info) {
    info->gameOver = 1;
    info->isInBattle = 0;
    clearTermninal();
    gotoxy(16,8);
    printf("GAME OVER\n               Your score was %d", *score);




}

//made by anders and jakob
//waits delay * 10 milliseconds
void wait(uint32_t delay){
    uint32_t cnt = 0;
    uint32_t start = getTimer15Cnt();
    while (cnt < start + delay) {
        cnt = getTimer15Cnt();
    }
}

//made by anders and jakob
int main(void)
{
    /////////////////////////initilazation of stuff START/////////////////////////
    struct GlobalInfo info = {0, 0, 0};

    uart_init(315200);
    homeCurser(); //Set curser til 0,0
    showCursor(0); //hide curser
    clearTermninal(); // Ryd terminal
    struct Map myMap = {malloc(8 * 33 * sizeof(uint8_t)), 32, {{' ',' ',' '},  {0xB2,0xB2,0xB2}, {',',' ','"'}, {'+', '+', '+'}}, {1,2,3,4}}; //R, W, G, L

    //random spawning health packs
    posXHPBOOZT = calloc(100, sizeof(uint8_t));
    posYHPBOOZT = calloc(100, sizeof(uint8_t));
    bOOZTuSED = calloc(100, sizeof(uint8_t));

    for(int i = 0; i < 100; i++){
        posXHPBOOZT[i] = rand() % 255;
        posYHPBOOZT[i] = rand() % 127;
    }

    //direction of van
    uint8_t dir = 0; //0 = front, 1 = back
    //lamp image for when in battle (data set later)
    struct Image lamp ={0, 8, 24};
    //van image for when in battle (data set later)
    struct Image van = {0,24,10};
    //the image data is using a lookup table to store color at what not

    uint16_t score = 0; //score of player added 1 for each lamppost

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

    //the image for the lamp
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
    //the image data for the van
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

    //sets the image data
    lamp.imageData  = &lamparr;
    van.imageData  = &vanarr;

    //player start position
    myMap.posX = 110;
    myMap.posY = 40;

    //temp storage for keycodes and what not
    char key;
    char nextKey;
    uint8_t bossEnable = 0;
    uint8_t helpEnable = 0;
    uint8_t startMenu = 1;
    uint8_t startMenuPrinted = 0;
    char radioToPrint[256];
    uint8_t lastReadJoy = 0;
    uint8_t turn = 0;

    //the LCD buffer
    uint8_t buf[512];
    lcd_graphics_buffer(buf, 512);

    //LCD Hover position
    uint8_t hoverPosition = 0;

    //struct to draw health in putty
    struct Fighter vanFighter = {255, "Van "};
    struct Fighter currentEnemy = {0, ""};

    //we setup PWM and timers
    setupRadio(radioToPrint);

    /////////////////////////initilazation of stuff END /////////////////////////

    //game logic loop
    while(1)
    {
        //dont inverse putty (was a problem at some point)
        inverse(0);

        //this stores what the next tile you land on will be
        uint8_t encounter = 0;

        //read keycodes
        nextKey = uart_get_char();
        uart_clear();
        if (nextKey != '\0') {
            if (nextKey == 'b')
                bossEnable = !bossEnable;
            if (nextKey == 'h')
                helpEnable = !helpEnable;

            key = nextKey;
        }
        if (startMenu) { //startmenu
            bgcolor(0);
            gotoxy(10,16);

            if (!startMenuPrinted){ //If startmenu hasn't been printed yet
            startMenuPrinted = 1; //Will only be printet once
            printf("Press 'p' to play.");
            gotoxy(5,18);
            printf("When in game, press 'h' for help.");
            }
            char key = uart_get_char();
            if (key == 'p') {
                startMenu = 0;
                clearTermninal();
            }
            uart_clear();
            gotoxy(0,0);
        }

        if (!startMenu) {
        //this is the game loop when you are in the open wild (not in battle)
            if(!info.isInBattle && !info.gameOver && !bossEnable && !helpEnable){

                //we reset the direction of the van when after a fight
                dir = 0;

                //this build all the buildings
                builds(&myMap);
                //this pleaces the health packs in the world
                placeHearts(&myMap);

                //needed to control the radio, see radio.c
                updateRadio(radioToPrint, buf);

                //prints the the view to the screen
                printSubMap(&myMap, 0,0, 32,32);
                //motion moves you if there is no wall and return the tile you are now standing on.
                encounter = motion(&myMap, key, &(vanFighter.hp));

                //we are standing on grass
                if (encounter == 2) {

                    //if you are stading on grass there will be a chance to encounter a wild lamppost, this checks if you have encountered one.
                    uint8_t foundWild = (rand() % 20 == 15); //10 % change to encounter wild pokemon

                    //if we have encountered one enter battle
                    if(foundWild){
                        info.isInBattle = 1; //we enter battle
                        bgcolor(0);
                        inverse(0);
                        currentEnemy.hp = 255; //sets the enemy hp to full
                        currentEnemy.name = "lamppost"; //sets the enemy to be a lamppost
                        clearTermninal(); //clears the terminal
                        lcd_graphics_buffer(buf, 512); //clears the LCD display

                    }
                }

            }
            else if(bossEnable || helpEnable){

                //if the bosskey is enabled, we only show the "sending important emails stuff" and update the radio
                updateRadio(radioToPrint, buf);
                keyCommands(&key, &bossEnable, &helpEnable);

            }
            else if (info.gameOver != 1) {

                //play battle music when in battle
                battleMusic();
                gotoxy(0,0);
                //print the image of the lamp
                printSubImage(&lamp, 0, 0, lamp.width, lamp.heigth, 25, 3);
                //print the image of the van
                printSubImage(&van, 0, 0, van.width, van.heigth, 2, 30);
                //draw health if van and lamp
                drawHealth(&currentEnemy, 6, 10);
                drawHealth(&vanFighter, 30, 35);

                //the action that we have selected from the LCD display
                uint8_t action = DO_NOTHING;

                //push the battle options to the LCD display
                lcd_battle(&buf, &dir, hoverPosition);
                lcd_push_buffer(&buf);

                //if it your turn, 0 = your turn 1 = lamppost turn
                if (turn == 0) {

                    gotoxy(15,28);
                    clreol();

                    //edge detection
                    if(readJoystick() != lastReadJoy){

                        //find what action needs to be taken.
                        action = updateHoverPosition(&hoverPosition, &dir);
                        lastReadJoy = readJoystick();
                    }
                    //if the action in frontal attack then we dmg the lamppost
                    if(action == FRONTAL_ATTACK){
                        uint8_t dmg = 20 + rand() % 10;
                        turn = !turn;// it will now be the lampposts turn

                        gotoxy(10,28);
                        printf("You crashed forwards into wild lamppost!");
                        gotoxy(10,29);
                        printf("Lamppost took %d damage!", dmg);
                        wait(150);


                        //if it is dies
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

                        wait(100); //waits 1 sec (10 * x ms)
                    }
                    else if(action == REVERSE_ATTACK){
                        //if we reverse attack we do more dmg
                        uint8_t dmg = 30 + rand() % 20;
                        turn = !turn; // it will now be the lampposts turn
                        ;
                        gotoxy(10,28);
                        printf("You reversed into wild lamppost!");
                        gotoxy(10,29);
                        printf("Lamppost took %d damage!", dmg);
                        wait(150);

                        //if we kill it
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


                        wait(200);//waits 2 sec (10 * x ms)
                    }
                    //horn does nothing
                    else if (action == HORN) {
                        turn = !turn;
                        gotoxy(10,28);
                        printf("You used your horn at wild lamppost!");
                        gotoxy(10,29);
                        printf("Attack was inefficient!");
                         wait(150);

                    }
                    //turn turns the car around
                    else if(action == TURN) {
                        dir = !dir;
                        turn = !turn;
                        gotoxy(10,28);
                        printf("You performed a three-point turn-around!");
                        wait(150);
                    }
                    //flee makes you take dmg and exit combat
                    else if(action == FLEE){

                        info.isInBattle = 0;
                        lcd_graphics_buffer(buf, 512);
                        lcd_push_buffer(buf);
                        clearTermninal();
                        if (vanFighter.hp > 20) {
                            vanFighter.hp -= 20;
                            gotoxy(10,28);
                            printf("You escaped!");
                            gotoxy(10,29);
                            printf("You took %d damage!", 20);
                            wait(150);
                        } else {
                            GameOver(&score, &info);
                        }
                    }

                gotoxy(10,28);
                clreol();
                gotoxy(10,29);
                clreol();


                }
                //this is the lamppost turn
                if (turn == 1 && info.isInBattle) {
                    turn = !turn;
                    gotoxy(10,28);
                    printf("Wild lamppost used 'blind'!"); //it allways uses blind
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

        }

        key = '\0';
        gotoxy(0,0);

    }
}
