#include "Utility.h"

//the the port mode of a pin, use the defines in Utility.h
void setPortMode(GPIO_TypeDef * GPIO, char pin, uint8_t mode)   {

    GPIO->MODER &= ~(0x00000003 << (pin * 2)); // Clear mode register
    GPIO->MODER |= (mode << (pin * 2)); // Set mode register (0x00 � Input, 0x01 - Output, 0x02 - Alternate Function, 0x03 - Analog in/out)

}

//setup pin pullup / pulldown, again use the defines in Utility.h
void setPortPuPd(GPIO_TypeDef * GPIO, char pin, uint8_t pupd) {//PullUp PullDown

    GPIO->PUPDR &= ~(0x00000003 << (pin * 2));
    GPIO->PUPDR |= ((pupd) << (pin * 2));
}

//read from a pin, use the defines in Utility.h
uint8_t readPortPin(GPIO_TypeDef * GPIO, char pin) {


    uint8_t val = GPIO->IDR & (0x0001 << pin); //pin<pin> AND'es med 1, alle andre AND'es med 0.
    return val != 0; //returner boolean true eller false (1/0).
}

//set pin output, use the defines in Utility.h
void setPin(GPIO_TypeDef * GPIO, char pin, uint8_t val) {

    GPIO->ODR &= ~(0x0001 << pin); //pin<pin> cleares til 0
    GPIO->ODR |= ((val != 0) << pin); //pin<pin> s�ttes til val
}

//sets the led to some color for some uint8_t (dont matter to much)
void setLED(uint8_t color){

    color = ~color; //LEDerne aktiveres ved at give dem ground, da de p� den modsatte side er fordundet til VDD. VDD->LED->GND

    setPortMode(BLUE_LED, OUT_MODE); //BLUE_LED = GPIOA, 9! Denne pin s�ttes som output. Kunne dette g�res i initialiseringen i stedet?? Anders
    setPortMode(GREEN_LED, OUT_MODE);
    setPortMode(RED_LED, OUT_MODE);

    setPin(BLUE_LED, (color & 1)); //Blue aktiveres n�r joystick er Up
    setPin(GREEN_LED, (color >> 1) & 1); //Joystick er Down
    setPin(RED_LED, (color >> 2) & 1); //Joystick er Left
    //Kunne man aktivere to farver p� �n gang til de resterende retninger ?? Anders
}

//return an uint8_t with the first bit being the value of up, the secound the value of down, the 3. is the value of left, the 4. is right and the 5. is center.
uint8_t readJoystick(){

    uint8_t val = 0;
    val |= (readPortPin(UP_JOY_STICK) << 0);
    val |= (readPortPin(DOWN_JOY_STICK) << 1);
    val |= (readPortPin(LEFT_JOY_STICK) << 2);
    val |= (readPortPin(RIGHT_JOY_STICK) << 3);
    val |= (readPortPin(CENTER_JOY_STICK) << 4);

    return val;
}

void setupTimer(TIM_TypeDef* TIM, uint32_t RCC_APBPeriph, uint16_t reloadValue, uint16_t prescale){
    //Opsetning af timer---------------------------
    RCC->APB1ENR |= RCC_APBPeriph; //Enable Clockline to timer.
    TIM->CR1 = 0x01; //Timer enabled, all other bits disabled
    TIM->ARR = reloadValue; //Reload value sat til 256-1.
    TIM->PSC = prescale; //Prescale sat til 2500-1 (maximum) (-1 pga formel)
}

void setupTimerInterupts(TIM_TypeDef* TIM, uint8_t TIM_IRQn,  uint8_t priority){
    //Interrupts------------------------
    TIM->DIER |= 0x0001; //Enable Timer 2 Interrupts
    NVIC_SetPriority(TIM_IRQn, priority); //Set Interrupt Priority
    NVIC_EnableIRQ(TIM_IRQn); //Enable interrupt
}
