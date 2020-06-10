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
    uart_init(9600);

    RCC->APB1ENR |= RCC_APB1Periph_TIM2; //Enable Clockline to timer 2.
    TIM2->CR1 = 0x01; //Timer enabled, all other bits disabled
    TIM2->ARR = 0x9C3; //Reload value sat til 2500-1.
    TIM2->PSC = 0xFFFE; //Prescale sat til 2^16-1 (maximum) (-1 pga formel)
    /* Udregninger:
    Periodetid = (1+ARR)*(1+PSC) / ClockFreq = 10ms, 1/100sekund
    */

  while(1)
  {
    int16_t sec;
    sec = second(TIM2);
    printf("%d\n", sec);
  }
}

int16_t second(TIM_TypeDef * timer) { //Ikke helt på plads
    int16_t val;
    val = timer->CNT;
    return val;
}
