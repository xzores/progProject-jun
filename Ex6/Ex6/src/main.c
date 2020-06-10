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
static uint8_t printSec = 0;

void TIM2_IRQHandler(void) {
    hundreths++;
    if (hundreths >= 100) {
        hundreths = 0;
        seconds++;
        printSec = 1;
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

    //Joystick pins---------------------
    RCC->AHBENR |= RCC_AHBPeriph_GPIOA; //Enabling clock for IO Port A
    RCC->AHBENR |= RCC_AHBPeriph_GPIOB;
    RCC->AHBENR |= RCC_AHBPeriph_GPIOC;
    RCC->AHBENR |= RCC_AHBPeriph_GPIOD;

    setPortMode(RIGHT_JOY_STICK, IN_MODE); //Porten RJS er forbundet til s�ttes til input-mode.
    setPortPuPd(RIGHT_JOY_STICK, NO_PULL); //Porten er forbundet til 1/0 i hardware, det er ikke n�dvendigt med pull ?? Anders

    setPortMode(UP_JOY_STICK, IN_MODE);
    setPortPuPd(UP_JOY_STICK, NO_PULL);

    setPortMode(CENTER_JOY_STICK, IN_MODE);
    setPortPuPd(CENTER_JOY_STICK, NO_PULL);

    setPortMode(LEFT_JOY_STICK, IN_MODE);
    setPortPuPd(LEFT_JOY_STICK, NO_PULL);

    setPortMode(DOWN_JOY_STICK, IN_MODE);
    setPortPuPd(DOWN_JOY_STICK, NO_PULL);


    printf("\n\n"); //space i terminalen
    printf("%d.%d.%d.%d\n", hours, minutes, seconds, hundreths);//0.0.0.0

    //Opsætning af timer---------------------------
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
    uint8_t joy = 0;
  while(1)
  {
    if(printSec) {
        printSec = 0;
        printf("%d.%d.%d.%d\n", hours, minutes, seconds, hundreths);
    }


    if (joy != readJoystick()) {
        if (joy == 0x10) {
            joy = 0;
            TIM2->CR1 = !TIM2->CR1;
            printf("h\n");
            }
        }
    }
}
