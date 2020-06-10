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

int32_t second(TIM_TypeDef * timer, int16_t sec) { //Ikke helt på plads
    if (timer->CNT >= timer->ARR) {
       sec++;
    }
    return sec;
}

static uint8_t hundreths = 0;
static uint8_t seconds = 0;
static uint8_t minutes = 0;
static uint8_t hours = 0;

void TIM2_IRQHandler(void) {
    hundreths++;
    if (hundreths >= 100) {
        hundreths = 0;
        seconds++;
    }

    if (seconds >= 60) {
        seconds = 0;
        minutes++;
    }

    if (minutes >= 60) {
        minutes = 0;
        hours++;
    }

    TIM2->SR &= ~0x0001; //Clear interrupt bit
}


int main(void) {
    uart_init(9600);

    RCC->APB1ENR |= RCC_APB1Periph_TIM2; //Enable Clockline to timer 2.
    TIM2->CR1 = 0x01; //Timer enabled, all other bits disabled
    TIM2->ARR = 0xFF; //Reload value sat til 256-1.
    TIM2->PSC = 0x9C3; //Prescale sat til 2500-1 (maximum) (-1 pga formel)
    //Interrupts------------------------
    TIM2->DIER |= 0x0001; //Enable Timer 2 Interrupts
    NVIC_SetPriority(TIM2_IRQn, 0x0); //Set Interrupt Priority
    NVIC_EnableIRQ(TIM2_IRQn); //Enable interrupt

    /* Udregninger:
    Periodetid = (1+ARR)*(1+PSC) / ClockFreq = 10ms, 1/100sekund
    */
    int16_t sec = 0;
    uint8_t lastSec = 0;

    printf("%d.%d.%d.%d\n", hours, minutes, seconds, hundreths);//0.0.0.0
  while(1)
  {
    //sec = second(TIM2, sec);
    if(seconds > lastSec){
        lastSec = seconds;
        printf("%d.%d.%d.%d\n", hours, minutes, seconds, hundreths);
    }

    if(lastSec >= 59){
        lastSec = 0;
    }

  }
}
