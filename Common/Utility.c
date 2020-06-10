

#include "Utility.h"

void setPortMode(GPIO_TypeDef * GPIO, char pin, uint8_t mode)   {

    GPIO->MODER &= ~(0x00000003 << (pin * 2)); // Clear mode register
    GPIO->MODER |= (mode << (pin * 2)); // Set mode register (0x00 – Input, 0x01 - Output, 0x02 - Alternate Function, 0x03 - Analog in/out)

}

void setPortPuPd(GPIO_TypeDef * GPIO, char pin, uint8_t pupd) {//PullUp PullDown

    GPIO->PUPDR &= ~(0x00000003 << (pin * 2));
    GPIO->PUPDR |= ((pupd) << (pin * 2));
}

uint8_t readPortPin(GPIO_TypeDef * GPIO, char pin) {

    uint8_t val = GPIO->IDR & (0x0001 << pin); //pin<pin> AND'es med 1, alle andre AND'es med 0.
    return val != 0; //returner boolean true eller false (1/0).
}

void setPin(GPIO_TypeDef * GPIO, char pin, uint8_t val) {

    GPIO->ODR &= ~(0x0001 << pin); //pin<pin> cleares til 0
    GPIO->ODR |= ((val != 0) << pin); //pin<pin> sættes til val
}

void setLED(uint8_t color){

    color = ~color; //LEDerne aktiveres ved at give dem ground, da de på den modsatte side er fordundet til VDD. VDD->LED->GND

    setPortMode(BLUE_LED, OUT_MODE); //BLUE_LED = GPIOA, 9! Denne pin sættes som output. Kunne dette gøres i initialiseringen i stedet?? Anders
    setPortMode(GREEN_LED, OUT_MODE);
    setPortMode(RED_LED, OUT_MODE);

    setPin(BLUE_LED, (color & 1)); //Blue aktiveres når joystick er Up
    setPin(GREEN_LED, (color >> 1) & 1); //Joystick er Down
    setPin(RED_LED, (color >> 2) & 1); //Joystick er Left
    //Kunne man aktivere to farver på én gang til de resterende retninger ?? Anders
}

uint8_t readJoystick(){

    uint8_t val = 0;
    val |= (readPortPin(UP_JOY_STICK) << 0);
    val |= (readPortPin(DOWN_JOY_STICK) << 1);
    val |= (readPortPin(LEFT_JOY_STICK) << 2);
    val |= (readPortPin(RIGHT_JOY_STICK) << 3);
    val |= (readPortPin(CENTER_JOY_STICK) << 4);
    // val = 000xxxxx, hvor et x bliver til 1 når joystick er aktiveret i den retning, og ellers 0.
    return val;
}
