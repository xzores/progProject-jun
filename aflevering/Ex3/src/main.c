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
#include "30010_io.h" // Input/output library for this course
#include "LUT.h"
#include "vector.h"

int main(void)
{

    uart_init( 9600 ); // Initialize USB serial at 9600 baud

    printFix(expand(mysin(0)));
    printFix(expand(mysin(64)));
    printFix(expand(mysin(-111)));
    printFix(expand(mysin(923)));

    printf("\n");

    printFix(expand(mycos(0)));
    printFix(expand(mycos(64)));
    printFix(expand(mycos(-111)));
    printFix(expand(mycos(923)));

    printf("\n");

    multiply(32, 8192);

    struct vector_t v = {1,2};
    rotateVector(&v, 12);

    //rotateVector((6,4), -10°) (6.5955, 2.9154)
    //rotateVector((-4,-4), 900°) (4.0000, 4.0000)
    //rotateVector((-4,2), -35°)

  while(1)
  {

  }
}
