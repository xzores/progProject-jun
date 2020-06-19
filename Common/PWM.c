#include "PWM.h"

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
void configCount(TIM_TypeDef* TIM)
{
    TIM->CCER &= ~TIM_CCER_CC3P; // Clear CCER register
    TIM->CCER |= 0x00000001 << 8; // Enable OC3 output
    TIM->CCMR2 &= ~TIM_CCMR2_OC3M; // Clear CCMR2 register
    TIM->CCMR2 &= ~TIM_CCMR2_CC3S;
    TIM->CCMR2 |= TIM_OCMode_PWM1; // Set output mode to PWM1
    TIM->CCMR2 &= ~TIM_CCMR2_OC3PE;
    TIM->CCMR2 |= TIM_OCPreload_Enable;
    TIM->CCR3 = 500; // Set duty cycle to 50 %
}

void setFreq(TIM_TypeDef* TIM, uint32_t freq) {

    uint32_t reload = 64e6 / freq / (9 + 1) - 1;

    TIM->ARR = reload; // Set auto reload value
    TIM->CCR3 = reload/2; // Set compare register

    TIM->EGR |= 0x01;

}



