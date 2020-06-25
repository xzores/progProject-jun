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
#include "Utility.h"

int main(void) {
    uart_init( 9600 );

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
        uint8_t val = readJoystick();
        setLED(val);
        printf("%d", val);
    }
}
