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

void TIM1_BRK_TIM15_IRQnHandle(void) {
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

    TIM15->SR &= ~0x0001; //Clear interrupt bit
}



void resetTimer(struct time timer) {
    timer.h = 0;
    timer.s = 0;
    timer.m = 0;
    timer.H = 0;
}


int main(void) {
    uart_init(9600);
    homeCurser(); //Sæt curser til 0,0
    clearTermninal(); // Ryd terminal

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
    gotoxy(5,5);
    printf("%d.%d.%d.%d", timer.H, timer.m, timer.s, timer.h);//0.0.0.0
    gotoxy(5,6);
    printf("Splittime #1:");
    gotoxy(5,7);
    printf("Splittime #2:");
    gotoxy(5,8);
    printf("Type 'Help' if u stupid");


    //Opsætning af timer---------------------------
    RCC->APB2ENR |= RCC_APB2Periph_TIM15; //Enable Clockline to timer 2.
    TIM15->CR1 = 0x01; //Timer enabled, all other bits disabled
    TIM15->ARR = 0xFF; //Reload value sat til 256-1.
    TIM15->PSC = 0x9C3; //Prescale sat til 2500-1 (maximum) (-1 pga formel)
    //Interrupts------------------------
    TIM15->DIER |= 0x0001; //Enable Timer 2 Interrupts
    NVIC_SetPriority(TIM1_BRK_TIM15_IRQn, 0x0); //Set Interrupt Priority
    NVIC_EnableIRQ(TIM1_BRK_TIM15_IRQn); //Enable interrupt

    uint8_t joy = 0; //Joystick edge-detection
    uint16_t readBytes = 0;
    resetTimer(timer);

    //Lav boks
    window(2,2,7,34,'B',"Timer");


    //Brugerinput init stuff------------------------------------

    char array_p[1024];



    /*
    uint8_t i;
    for (i = 0; i < 5; i++) {
        printf("%s", *array_p);
        array_p++;
    }
    int result;
    result = compare_string(array_p, "Start"); //Sammenligning af strings
    printf("%d\n", result);
    */


  while(1)
  {
    gotoxy(5,5);
    if(printSec) {
        printSec = 0;
        printf("%02d.%02d.%02d.--", timer.H, timer.m, timer.s);
    }

    gotoxy(20,5);


    if (joy != readJoystick()) {
        joy = readJoystick();
        if (joy == 0x10) {
            TIM15->CR1 = !TIM15->CR1;
            }
        else if (joy == 0x04) {// joy readBytesleft: Split 1
            gotoxy(22,6);
            printf("%d.%d.%d.%d", timer.H, timer.m, timer.s,timer.h);

        }

        else if (joy == 0x08) {//joy right: Split 2, kun hvis split 1 allerede eksisterer
            gotoxy(22,7);
            printf("%d.%d.%d.%d", timer.H, timer.m, timer.s,timer.h);

        }

        else if (joy == 0x02) { //Joy-Down: stop og reset
            TIM15->CR1 = 0x00; //Sluk timer
            TIM15->CNT = 0x00; //reset counter til 0.
            resetTimer(timer);

            gotoxy(0,2);
            printf("%d.%d.%d.%d", timer.H, timer.m, timer.s,timer.h);

            gotoxy(0,3);
            printf("%d.%d.%d.%d", timer.H, timer.m, timer.s,timer.h);

            }
            gotoxy(20,5);
        }

        // Bruger input af tekst--------------------
        //gotoxy(1,5);
        gotoxy(20,5);
        printf("|");
        gotoxy(21 + readBytes,5);//Brugerinputtet skulle nu være på array[i], hvor pegeren peger.
        uint16_t rB = readFromTerminal(array_p, 10);
        if(readBytes > rB){
            gotoxy(21,5);//Brugerinputtet skulle nu være på array[i], hvor pegeren peger.
            printf("          ");
        }
        readBytes = rB;

        uint8_t didSomething = 1;

        if(!strcmp(array_p, "Start") && readBytes == 0){
            TIM15->CR1 = 0x01; //timer enable
            }
        else if (!strcmp(array_p, "Stop") && readBytes == 0) {
            TIM15->CR1 = 0x00; //Timer disable
        }
        else if (!strcmp(array_p, "Split1") && readBytes == 0) {
            gotoxy(22,6);
            printf("%d.%d.%d.%d", timer.H, timer.m, timer.s,timer.h);
            resetInput(array_p);
        }
        else if (!strcmp(array_p, "Split2") && readBytes == 0) {
            gotoxy(22,7);
            printf("%d.%d.%d.%d", timer.H, timer.m, timer.s,timer.h);
        }
        else if (!strcmp(array_p, "Reset") && readBytes == 0) {
            TIM15->CR1 = 0x00; //Sluk timer
            TIM15->CNT = 0x00; //reset counter til 0.
            resetTimer(timer);

            gotoxy(0,2);
            printf("%d.%d.%d.%d", timer.H, timer.m, timer.s,timer.h);

            gotoxy(0,3);
            printf("%d.%d.%d.%d", timer.H, timer.m, timer.s,timer.h);
        }
        else if (!strcmp
                (array_p, "Help") && readBytes == 0) {
            gotoxy(3,12);
            printf("U stupid\nType 'Start' or 'Stop' to start/stop watch,\n and 'Split1' or '-2' for splittimes!");
        }
        else{

            didSomething = 0;
        }

        if(didSomething)
            array_p[0] = '\0';

        //if(readFromTerminal(array_p, ))
        //printf("done %s", array_p);
    }

}
