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
#include "LUT.h"
#include "vector.h"

//print a fix point number
void printFix(int32_t i) {
 // Prints a signed 16.16 fixed point number
 if ((i & 0x80000000) != 0) { // Handle negative numbers
 printf("-");
 i = ~i + 1;
 }
 printf("%ld.%04ld\n", i >> 16, 10000 * (uint32_t)(i & 0xFFFF) >> 16);
 // Print a maximum of 4 decimal digits to avoid overflow
 }

//turns a 18.14 to a 16.16 (or 2.14 to 16.16)
int32_t expand(int32_t i) {
    // Converts an 18.14 fixed point number to 16.16
    return i << 2;
}

//self explanatory
int16_t abs(int16_t k){
    if(k >= 0)
        return k;
    else
        return -k;
}

//returns the value sin to a 512 angle system
int16_t mysin(int16_t angle){
    uint16_t index = 0;

    //handels negative angles
    if (angle >= 0)
        index = (angle % 512);
    else
        index = 512 + (angle % 512); //the % operator is used to loop the value between 0 and 512

    return SIN[index]; //get the value from the look up table
}

//returns the value sin to a 512 angle system
int16_t mycos(int16_t angle){
    return mysin(angle + (512 / 4));
}

//unused, but sets the value of a vector to (10,20)
void initVector(struct vector_t* v) {
    v->x = 10;
    v->y = 20;
}

//once only me and god knew how it worked, now it is only god, if you change this and fail increase the counter: 42
//this multiplies a unsigned it with a 16.16 fixed point number. (might be changed to multiply a 16.16 fixed point number with a 16.16 fixed point number)
int32_t multiply(int32_t a, int32_t b){

    int32_t res = 0;

    int32_t deci = b & 0x0000FFFF;
    int32_t inters = b & 0xFFFF0000;

    res = a * (inters >> 16);
    res += (a * (deci)) >> 16;

    return res;
}

//rotates a vector by the angle
void rotateVector(struct vector_t* v, int16_t angle) {
    struct vector_t v_temp = {v->x, v->y};
    v->x = multiply(v_temp.x, expand(mycos(angle))) + multiply(v_temp.y, expand(mysin(angle)));
    v->y = multiply(v_temp.x, expand(mysin(angle))) + multiply(v_temp.y, expand(mycos(angle)));
}
