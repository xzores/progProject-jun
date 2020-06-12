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

void lcd_update(){


}

int main(void)
{
    lcd_init(); //self explanatory
    uint8_t buf[512]; //the display buffer
    lcd_graphics_buffer(buf);  //init the display buffer
    lcd_write_string(buf, "minecraft", 10,2); //print mincraft to it.

    while(1)
    {

    }
}
