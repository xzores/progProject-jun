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

struct time{
uint8_t h;
uint8_t s;
uint8_t m;
uint8_t H;
} time;

struct time timer;

static uint8_t printSec = 0;


void TIM2_IRQHandler(void) {
    timer.h++;
    if (timer.h >= 100) {
        timer.h = 0;
        timer.s++;
        printSec = 1;
    }

    if (timer.s >= 60) {
        timer.s = 0;
        timer.m++;
    }

    if (timer.m >= 60) {
        timer.m = 0;
        timer.H++;
    }

    TIM2->SR &= ~0x0001; //Clear interrupt bit
}



void resetTimer(struct time timer) {
    timer.h = 0;
    timer.s = 0;
    timer.m = 0;
    timer.H = 0;
}


int main(void) {
    uart_init(9600);
    clearTermninal();
    //Joystick pins---------------------
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

    printf("%d.%d.%d.%d", timer.H, timer.m, timer.s, timer.h);//0.0.0.0


    //Opsætning af timer---------------------------
    RCC->APB1ENR |= RCC_APB1Periph_TIM2; //Enable Clockline to timer 2.
    TIM2->CR1 = 0x01; //Timer enabled, all other bits disabled
    TIM2->ARR = 0xFF; //Reload value sat til 256-1.
    TIM2->PSC = 0x9C3; //Prescale sat til 2500-1 (maximum) (-1 pga formel)
    //Interrupts------------------------
    TIM2->DIER |= 0x0001; //Enable Timer 2 Interrupts
    NVIC_SetPriority(TIM2_IRQn, 0x0); //Set Interrupt Priority
    NVIC_EnableIRQ(TIM2_IRQn); //Enable interrupt

    uint8_t joy = 0; //Joystick edge-detection

    resetTimer(timer);

  while(1)
  {
    gotoxy(0,0);
    if(printSec) {
        printSec = 0;
        printf("%d.%d.%d.--", timer.H, timer.m, timer.s);
        gotoxy(0,0);
    }


    if (joy != readJoystick()) {
        joy = readJoystick();
        if (joy == 0x10) {
            TIM2->CR1 = !TIM2->CR1;
            }
        else if (joy == 0x04) {// joy left: Split 1
            gotoxy(0,2);
            printf("%d.%d.%d.%d", timer.H, timer.m, timer.s,timer.h);
            homeCurser();
        }

        else if (joy == 0x08) {//joy right: Split 2, kun hvis split 1 allerede eksisterer
            gotoxy(0,3);
            printf("%d.%d.%d.%d", timer.H, timer.m, timer.s,timer.h);
            homeCurser();
        }

        else if (joy == 0x02) { //Joy-Down: stop og reset
            TIM2->CR1 = 0x00; //Sluk timer
            TIM2->CNT = 0x00; //reset counter til 0.
            resetTimer(timer);

            gotoxy(0,2);
            printf("%d.%d.%d.%d", timer.H, timer.m, timer.s,timer.h);

            gotoxy(0,3);
            printf("%d.%d.%d.%d", timer.H, timer.m, timer.s,timer.h);

            }

        }
    }

}
