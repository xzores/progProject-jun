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
#include "stm32f30x.h"
#include "30010_io.h"
#include "Utility.h"
#include "PWM.h"


struct time{
uint8_t h;
uint8_t s;
uint8_t m;
uint8_t H;
} time;

static struct time timer;

static uint32_t notes[] = {100, 0, 1000, 0, 1000, 0};
uint8_t nindex = 0;

void TIM1_BRK_TIM15_IRQnHandle(void){
    uint32_t t = notes[nindex % 6];
    setFreq(TIM2, t);
    nindex++;
    TIM15->SR &= ~0x0001; //Clear interrupt bit
}

/*
void TIM1_BRK_TIM15_IRQnHandler() {
    timer.h++;
    if (timer.h >= 100) {
        timer.h = 0;
        timer.s++;

    }

    if (timer.s >= 60) {
        timer.s = 0;
        timer.m++;
    }

    if (timer.m >= 60) {
        timer.m = 0;
        timer.H++;
    }

    TIM15->SR &= ~0x0001; //Clear interrupt bit
}
*/


int main(void)
{

/* LCD stuff ------------------------------------------------------------------*/
/*
    lcd_init();
    uint8_t buf[512*2]; // the internal buffer
    uint8_t shiftBuf[512]; // the display buffer
    lcd_graphics_buffer(buf, 512*2); //init the internal buffer
    lcd_write_string(buf, "FM", 10,2); //write to the internal buffer
    //lcd_write_string(buf, "hurdisalt", 5,0); //write to the internal buffer

    lcd_shift_right(buf, shiftBuf, 10); //copy from internal buffer to display buffer at location.
    lcd_push_buffer(shiftBuf); // push view

    resetTimer(); //setup timer
    configT15();
    setupTimer(TIM, RCC_APB1Periph_TIM2, 0xFF, 0x9C3); //setup timer
    setupTimerInterupts(TIM15, TIM15_IRQ, 1); //setup timer
*/


/* Sound stuff ------------------------------------------------------------------*/

    RCC->AHBENR |= RCC_AHBENR_GPIOBEN; // Enable clock line for GPIO bank B
    GPIOB->MODER &= ~(0x00000003 << (10 * 2)); // Clear mode register
    GPIOB->MODER |= (0x00000002 << (10 * 2)); // Set mode register
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_1);

    configT15();

    //timer  2 for PWM
    setupTimer(TIM2, RCC_APB1Periph_TIM2, 63999, 9);
    configCount(TIM2);

    //default freq
    setFreq(TIM2, 1000);


  while(1)
  {




  }
}
