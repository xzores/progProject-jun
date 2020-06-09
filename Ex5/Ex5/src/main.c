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

int main(void) {
    uart_init( 9600 );

    setPortMode(GPIOA, 1, GPIO_IN)




  while(1)
  {

  }
}

#define IN_MODE 0x00000000
#define OUT_MODE 0x00000001
#define ALT_MODE 0x00000002
#define ANAL_MODE 0x00000003

void setPortMode(GPIO_TypeDef * GPIO, char pin, uint32_t mode)   {

    GPIO->MODER &= ~(0x00000003 << (pin * 2)); // Clear mode register
    GPIO->MODER |= (mode << (pin * 2)); // Set mode register (0x00 – Input, 0x01 - Output, 0x02 - Alternate Function, 0x03 - Analog in/out)


    }

void setPortPuPd(GPIO_TypeDef * GPIO, char pin, uint32_t pupd) {//PullUp PullDown

    int16_t hexCode;

    if(pupd == 0) {
        hexCode = 0x02; //pull down
    }
    else if (pupd == 1) {
        hexCode = 0x01; //pull up
    }
    else {
        hexcode = 0x00; //no pull
    }

    GPIO->PUPDR &= ~(0x00000003 << (pin * 2));
    GPIO->PUPDR |= ((hexCode) << (pin * 2));
}

void readPortPin(GPIO_TypeDef * GPIO, char pin, uint32_t pupd) {//PullUp PullDown

    uint16_t val = GPIO->IDR & (0x0001 << 0); //Read from pin PA0
}
