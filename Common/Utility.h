#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED

#include "stm32f30x_conf.h"
#include "30010_io.h"

#define IN_MODE 0x00000000
#define OUT_MODE 0x00000001
#define ALT_MODE 0x00000002
#define ANAL_MODE 0x00000003

#define NO_PULL 0x00000000
#define PULL_UP 0x00000001
#define PULL_DOWN 0x00000002

#define RIGHT_JOY_STICK GPIOC, 0
#define UP_JOY_STICK GPIOA, 4
#define CENTER_JOY_STICK GPIOB, 5
#define LEFT_JOY_STICK GPIOC, 1
#define DOWN_JOY_STICK GPIOB, 0

#define BLUE_LED GPIOA, 9
#define GREEN_LED GPIOC, 7
#define RED_LED GPIOB, 4

//self explanatory
inline int32_t power(int32_t a, int32_t exp) {
// calculates a^exp
 int32_t i, r = 1;
 for (i = 1; i <= exp; i++)
 r *= a;
 return(r);
}

void setPortMode(GPIO_TypeDef * GPIO, char pin, uint8_t mode);
void setPortPuPd(GPIO_TypeDef * GPIO, char pin, uint8_t pupd);
uint8_t readPortPin(GPIO_TypeDef * GPIO, char pin);
void setPin(GPIO_TypeDef * GPIO, char pin, uint8_t val);
void setLED(uint8_t color);
uint8_t readJoystick();
void setupTimer(TIM_TypeDef* TIM, uint32_t RCC_APBPeriph, uint16_t reloadValue, uint16_t prescale);
void setupTimerInterupts(TIM_TypeDef* TIM, uint8_t TIM_IRQn,  uint8_t priority);


#endif
