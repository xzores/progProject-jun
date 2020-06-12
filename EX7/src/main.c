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


struct time{
uint8_t h;
uint8_t s;
uint8_t m;
uint8_t H;
};

struct time timer;

static uint8_t printSec = 0;
static uint8_t printClear = 0;

void TIM2_IRQHandler(void) {
    timer.h++;
    printSec = (timer.h % 10) == 0;
    printClear = (timer.h % 10) != 0;
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

    TIM2->SR &= ~0x0001; //Clear interrupt bit
}

void resetTimer() {
    timer.h = 0;
    timer.s = 0;
    timer.m = 0;
    timer.H = 0;
}

static int i = 0;

void lcd_update(uint8_t* buf, uint8_t* shiftBuf){

    lcd_shift_right(buf, shiftBuf, i);
    lcd_push_buffer(shiftBuf);

    if(printSec){
        printSec = 0;
        lcd_write_string(buf, "hello world", 5,1);
        i++;
    }
     if(printClear){
        printClear = 0;
        lcd_write_string(buf, "           ", 5,1);
    }
}

int main(void)
{
    lcd_init();
    uint8_t buf[512*2]; // the internal buffer
    uint8_t shiftBuf[512]; // the display buffer
    lcd_graphics_buffer(buf, 512*2); //init the internal buffer
    lcd_graphics_buffer(shiftBuf, 512); //init the display buffer
    lcd_write_string(buf, "minecraft", 10,2); //write to the internal buffer
    lcd_write_string(buf, "hurdisalt", 5,0); //write to the internal buffer
    lcd_write_string(buf, "Hej, dette er longer than the display", 5,0); // write to the internal displat
    lcd_shift_right(buf, shiftBuf, 10); //copy from internal buffer to display buffer at location.
    lcd_push_buffer(shiftBuf); // push view

    resetTimer(); //setup timer
    setupTimer(TIM2, RCC_APB1Periph_TIM2, 0xFF, 0x9C3); //setup timer
    setupTimerInterupts(TIM2, TIM2_IRQn, 1); //setup timer


    while(1)
    {
        lcd_update(buf, shiftBuf);
    }
}
