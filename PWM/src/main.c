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


void setFreq(uint32_t freq) {

    uint32_t reload = 64e6 / freq / (9 + 1) - 1;

    for(freq=20; freq<=800 ;freq++){

        TIM2->ARR = reload; // Set auto reload value
        TIM2->CCR3 = reload/2; // Set compare register

        TIM2->EGR |= 0x01;

    }

}

void configT15()
{
    RCC->APB2ENR |= RCC_APB2Periph_TIM15; // Enable clock line to timer 2;
    TIM15->CR1 = 0x0000;
    TIM15->ARR = 6399; // Set auto reload value
    TIM15->PSC = 2499; // Set pre-scaler value
    TIM15->DIER |= 0x0001; // Enable timer interrupt

    NVIC_SetPriority(TIM1_BRK_TIM15_IRQn, 0);
    NVIC_EnableIRQ(TIM1_BRK_TIM15_IRQn);

    TIM15->CR1 |= 0x0001; // Enable timer

}

static volatile uint8_t notes[] = {100, 200, 500, 100, 100, 200, 1000};
static volatile uint16_t f = 0;

void TIM1_BRK_TIM15_IRQHandler (void){
    setFreq(notes[f++ % sizeof(notes)]);
    TIM15->SR &= ~0x0001; //Clear interrupt bit
}

void configT2()
{
    RCC->APB1ENR |= 0x00000001; // Enable clock line to timer 2;
    TIM2->CR1 = 0x0000; // Disable timer
    TIM2->ARR = 63999; // Set auto reload value
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

    RCC->AHBENR |= RCC_AHBENR_GPIOBEN; // Enable clock line for GPIO bank B
    GPIOB->MODER &= ~(0x00000003 << (10 * 2)); // Clear mode register
    GPIOB->MODER |= (0x00000002 << (10 * 2)); // Set mode register
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_1);

    //setupTimer(TIM2, RCC_APB1ENR_TIM2EN, 1000, 4);
    configT15();
    configT2();
    configCount();

    setFreq(100);


  while(1)
  {




  }
}
