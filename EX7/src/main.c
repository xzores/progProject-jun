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
#include "30010_io.h"


int main(void)
{
   lcd_init();
   lcd_graphics_buffer();
   lcd_write_string();
    while(1)
    {

    }
}
