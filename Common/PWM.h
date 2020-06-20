#ifndef PWM_H_INCLUDED
#define PWM_H_INCLUDED

/* Includes ------------------------------------------------------------------*/
#include "stm32f30x_conf.h"
#include "stm32f30x.h"
#include "30010_io.h"
#include "Utility.h"

void configT15();
void configT2();
void configCount(TIM_TypeDef*);
void setFreq(TIM_TypeDef*, uint32_t freq);
void TIM1_BRK_TIM15_IRQnHandler();
void resetTimer(struct time timer);


#endif  /*PWM_H_INCLUDED */
