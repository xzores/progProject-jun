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
#include "vector.h"
#include "ansi.h"
#include "30010_io.h"

int main(void)
{
    uart_init( 9600 );
    clearTermninal();
    inverse(0);

    window(10,10,25,50,'B',"");

  while(1)
  {

  }
}
