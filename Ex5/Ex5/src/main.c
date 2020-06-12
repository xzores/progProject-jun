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

#define IN_MODE 0x00000000
#define OUT_MODE 0x00000001
#define ALT_MODE 0x00000002
#define ANAL_MODE 0x00000003

#define NO_PULL 0x00000000
#define PULL_UP 0x00000001
#define PULL_DOWN 0x00000002

#define RIGHT_JOY_STICK GPIOC, 0
#define UP_JOY_STICK GPIOA, 4
#define CENTER_JOY_STICK GPIOB, 5
#define LEFT_JOY_STICK GPIOC, 1
#define DOWN_JOY_STICK GPIOB, 0

#define BLUE_LED GPIOA, 9
#define GREEN_LED GPIOC, 7
#define RED_LED GPIOB, 4

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
