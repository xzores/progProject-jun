
#include "stm32f30x_conf.h"
#include "30010_io.h"

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
