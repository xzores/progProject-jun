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
#include <stdlib.h>
#include <string.h>


struct time{
uint8_t h;
uint8_t s;
uint8_t m;
uint8_t H;
} time;

struct time timer;

static uint8_t printSec = 0;

//6.2---------------------------------------


uint16_t readFromTerminal(char * s, uint16_t limit) { //Tager en pointer til et chararray
    //uart_clear();
    static uint16_t i;

    char c = uart_get_char(); //Første char indlæses i arrayet
    while (c != '\0') { // 0 eller enter

        if(c == '\r' || i >= limit){
            s[i] = '\0'; //Der sættes \0 på første index efter input-string.
            i = 0;
            return i;
        }

        s[i] = c; //Værdien på adressen s[i] sættes til værdi af typen char.
        printf("%c",c); //printer nuværende string
        i++;
        c = uart_get_char();
    }

    return i;
}

void resetInput(char * s) {
    uint8_t j;
    while(s[j] != '\0') {
        s[j] = 0;
        j++;
    }
}

int compare_string(char *first, char *second)
{
   while(*first==*second)
   {
      if ( *first == '\0' || *second == '\0' )
         break;

      first++;
      second++;
   }
   if( *first == '\0' && *second == '\0' )
      return 0;
   else
      return -1;
}

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

    RCC->APB2ENR |= RCC_APB2ENR_TIM15EN; // Enable clock line to timer 2;
    TIM15->CR1 = 0x0000; // Disable timer
    TIM15->ARR = 250; // Set auto reload value
    TIM15->PSC = 2; // Set pre-scaler value

    TIM15->DIER |= 0x0001; // Enable timer 2 interrupts


    TIM15->CR1 |= 0x0001; // Enable timer


    while(1){};

}
