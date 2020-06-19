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


struct time{
uint8_t h;
uint8_t s;
uint8_t m;
uint8_t H;
} time;

static struct time timer;

static volatile uint32_t notes[] = {10000, 0, 5000, 0, 1000, 0};
volatile uint8_t nindex = 0;

void setFreq(uint32_t freq) {

    uint32_t reload = 64e6 / freq / (9 + 1) - 1;

    TIM2->ARR = reload; // Set auto reload value
    TIM2->CCR3 = reload/2; // Set compare register

    TIM2->EGR |= 0x01;

}


void TIM1_BRK_TIM15_IRQnHandle(void){
    //uint32_t t = notes[nindex % 6];
    //setFreq(50);
    nindex++;
    setFreq(10);
    TIM2->SR &= ~0x0001; //Clear interrupt bit
}

void configT15()
{
    RCC->APB2ENR |= RCC_APB2Periph_TIM15; // Enable clock line to timer 2;
    TIM15->CR1 = 0x0000;
    TIM15->ARR = 63999; // Set auto reload value
    TIM15->PSC = 9; // Set pre-scaler value
    TIM15->DIER |= 0x0001; // Enable timer interrupt
    NVIC_SetPriority(TIM1_BRK_TIM15_IRQn, 0);
    NVIC_EnableIRQ(TIM1_BRK_TIM15_IRQn);
    TIM15->CR1 |= 0x0001; // Enable timer
}

/*
void configT15()
{
    RCC->APB2ENR |= RCC_APB2Periph_TIM15; // Enable clock line to timer 2;
    TIM15->CR1 = 0x0000;
    TIM15->ARR = 25599; // Set auto reload value
    TIM15->PSC = 0x9C3; // Set pre-scaler value
    TIM15->DIER |= 0x0001; // Enable timer interrupt

    NVIC_SetPriority(TIM1_BRK_TIM15_IRQn, 1);
    NVIC_EnableIRQ(TIM1_BRK_TIM15_IRQn);

    TIM15->CR1 |= 0x0001; // Enable timer
    TIM2->SR &= ~0x0001; //Clear interrupt bit

}
*/

void configT2a()
{
    RCC->APB1ENR |= 0x00000001; // Enable clock line to timer 2;
    TIM2->CR1 = 0x0000; // Disable timer
    TIM2->ARR = 25599; // Set auto reload value
    TIM2->PSC = 9; // Set pre-scaler value
    TIM2->CR1 |= 0x0001; // Enable timer
}

void configCount()
{
    TIM2->CCER &= ~TIM_CCER_CC3P; // Clear CCER register
    TIM2->CCER |= 0x00000001 << 8; // Enable OC3 output
    TIM2->CCMR2 &= ~TIM_CCMR2_OC3M; // Clear CCMR2 register
    TIM2->CCMR2 &= ~TIM_CCMR2_CC3S;
    TIM2->CCMR2 |= TIM_OCMode_PWM1; // Set output mode to PWM1
    TIM2->CCMR2 &= ~TIM_CCMR2_OC3PE;
    TIM2->CCMR2 |= TIM_OCPreload_Enable;
    TIM2->CCR3 = 500; // Set duty cycle to 50 %
}


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

    //RCC->AHBENR |= RCC_AHBENR_GPIOBEN; // Enable clock line for GPIO bank B
    //GPIOB->MODER &= ~(0x00000003 << (10 * 2)); // Clear mode register
    //GPIOB->MODER |= (0x00000002 << (10 * 2)); // Set mode register
    //GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_1);

    //configT2a();
    //configCount();

    configT15();


    //default freq
    //setFreq(10);


  while(1)
  {




  }
}
