
#include "Radio.h"

void io_config()
 {
    RCC->AHBENR |= RCC_AHBPeriph_GPIOA; // Enable clock for GPIO Port A
    RCC->CFGR2 &= ~RCC_CFGR2_ADCPRE12; // Clear ADC12 prescaler bits
    RCC->CFGR2 |= RCC_CFGR2_ADCPRE12_DIV6; // Set ADC12 prescaler to 6
    RCC->AHBENR |= RCC_AHBPeriph_ADC12; // Enable clock for ADC12

    ADC1->CR = 0x00000000; // Clear CR register
    ADC1->CFGR &= 0xFDFFC007; // Clear ADC1 config register
    ADC1->SQR1 &= ~ADC_SQR1_L; // Clear regular sequence register 1

    ADC1->CR |= 0x10000000; // Enable internal ADC voltage regulator
    for (int i = 0 ; i < 1000 ; i++) {} // Wait for about 16 microseconds

    ADC1->CR |= 0x80000000; // Start ADC1 calibration
    while (!(ADC1->CR & 0x80000000)); // Wait for calibration to finish
    for (int i = 0 ; i < 100 ; i++) {} // Wait for a little while


    ADC1->CR |= 0x00000001; // Enable ADC1 (0x01 - Enable, 0x02 - Disable)
    while (!(ADC1->ISR & 0x00000001)); // Wait until ready
 }

  uint16_t readAdc1()
  {
    //setPin(GPIOA, 0, 1);
    setPortMode(GPIOA, 0, IN_MODE);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_1Cycles5);
    ADC_StartConversion(ADC1); // Start ADC read
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == 0);
    return ADC_GetConversionValue(ADC1); // Read the ADC value
}

uint16_t readAdc2()
{
    //setPin(GPIOA, 1, 1);
    setPortMode(GPIOA, 1, IN_MODE);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_1Cycles5);
    ADC_StartConversion(ADC1); // Start ADC read
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == 0);
    return ADC_GetConversionValue(ADC1); // Read the ADC value

}

void setFreq(uint32_t freq) {
    uint32_t v = readAdc2();
    uint32_t reload = 64e6 / freq / (9 + 1) - 1;

    TIM2->ARR = reload; // Set auto reload value
    TIM2->CCR3 = reload * 4096 / v / 2; // Set compare register

    TIM2->EGR |= 0x01;

}

void configT15(uint16_t BPM)
{

    RCC->APB2ENR |= RCC_APB2Periph_TIM15; // Enable clock line to timer 2;
    TIM15->CR1 = 0x0000;
    TIM15->ARR = 25599 / BPM* 60; // Set auto reload value 25599 / BPM
    TIM15->PSC = 2499; // Set pre-scaler value
    TIM15->DIER |= 0x0001; // Enable timer interrupt

    NVIC_SetPriority(TIM1_BRK_TIM15_IRQn, 5);
    NVIC_EnableIRQ(TIM1_BRK_TIM15_IRQn);

    TIM15->CR1 |= 0x0001; // Enable timer

}

//Coffin Dance------------------------------------------------------------------------------------------------
static volatile uint16_t notes[] = {466, 466, 466, 466, 587, 587, 587, 587, 523, 523, 523, 523, 689, 689, 689,
689, 783, 783, 783, 783, 783, 783, 783, 783, 783, 783, 783, 783, 523, 466, 440, 349, 392, 196, 196, 587, 523, 261,
466, 233, 440, 220, 220, 440, 523, 261, 466, 440, 392, 196, 392, 923, 880, 923, 880, 923, 392, 196, 392, 923, 880,
923, 880, 923, 392, 196, 196, 587, 523, 261, 466, 233, 440, 220, 220, 440, 523, 261, 466, 440, 392, 196, 392, 923,
880, 923, 880, 923, 392, 196, 392, 923, 880, 923, 880, 923, 392, 196, 196, 587, 523, 261,
466, 233, 440, 220, 220, 440, 523, 261, 466, 440, 392, 196, 392, 923, 880, 923, 880, 923, 392, 196, 392, 923, 880,
923, 880, 923, 392, 196, 196, 587, 523, 261, 466, 233, 440, 220, 220, 440, 523, 261, 466, 440, 392, 196, 392, 923,
880, 923, 880, 923, 392, 196, 392, 923, 880, 923, 880, 923};
static volatile uint16_t f = 0;
//-------------------------------------------------------------------------------------------------------------

//Lambada--------------------------------------------------------------------------------------------------------------
static volatile uint16_t notes2[]= {880, 880, 146, 783, 698, 659, 587, 146, 587, 698, 659, 587, 523, 587, 440, 392,
440, 87, 440, 87, 440, 87, 440, 87, 880, 146, 880, 783, 698, 659, 587, 146, 587, 698, 659, 587, 523, 587, 440, 392,
440, 87, 440, 87, 440, 87, 440, 87, 783, 98, 783, 698, 466, 98, 466, 587, 880, 98, 783, 698, 466, 98, 587, 698,
659, 130, 587, 523, 130, 523, 587, 523, 587, 146, 587, 146, 587, 146, 587, 146, 783, 98, 783, 698, 466, 98, 98, 587,
98, 880, 783, 698, 466, 98, 587, 698, 659, 130, 587, 523, 130, 523, 587, 523, 698, 659, 587, 146, 587, 146, 587, 146};
//-----------------------------------------------------------------------------------------------------------------------

//Tetris-----------------------------------------------------------------------------------------------------------------
static volatile uint16_t notes3[]= {659, 82, 493, 523, 587, 82, 523, 493, 440, 110, 440, 523, 659, 110, 587, 523, 493, 103,
493, 523, 587, 82, 659, 82, 523, 110, 440, 110, 440, 110, 123, 130, 146, 587, 73, 698, 880, 73, 783, 698, 659, 65, 659,
523, 659, 65, 587, 523, 493, 49, 493, 523, 587, 103, 659, 103, 523, 110, 440, 110, 440, 55, 110, 220};


//Pokemon Fight----------------------------------------------------------------------------------------------------------
static volatile uint16_t notes4[]= {246, 233, 220, 207, 233, 220, 207, 196, 220, 207, 196, 185, 207, 196, 185, 174, 196, 185,
174, 164, 185, 174, 164, 155, 174, 164, 155, 146, 164, 155, 146, 138, 493, 123, 123, 123, 146, 146, 164, 164, 123, 123,
174, 174, 349, 164, 146, 146, 293, 123, 329, 123, 146, 146, 349, 164, 123, 123, 146, 146, 261, 110, 130, 130,
493, 123, 123, 123, 146, 146, 164, 164, 123, 123, 174, 174, 349, 164, 146, 146, 293, 329, 123, 123, 146, 146, 349, 164,
123, 123, 146, 146, 261, 110, 130, 130, 493, 123, 123, 123, 146, 146, 164, 164, 123, 123,
174, 174, 349, 164, 146, 146, 293, 123, 329, 123, 146, 146, 164, 164, 123, 123, 261, 146, 440, 110, 130, 130,
493, 123, 123, 123, 146, 146, 164, 164, 123, 123, 174, 174, 349, 164, 146, 146, 293, 123, 329, 123, 146, 146, 349, 164,
123, 123, 146, 146, 261, 110, 130, 130, 123, 123, 185, 185, 123, 123, 185, 185, 123, 123, 185, 185, 123, 123,
185, 185, 123, 123, 185, 185, 123, 123, 185, 185, 123, 123, 185, 185, 123, 123, 185, 185, 130, 130, 196, 196, 130, 130,
196, 196, 130, 130, 196, 196, 130, 130, 196, 196, 130, 130, 196, 196, 130, 130, 196, 196, 130, 130, 196, 196, 130, 130, 196, 196,
123, 123, 185, 185, 123, 123, 185, 185, 123, 123, 185, 185, 123, 123, 185, 185, 123, 123, 185, 185, 123, 123, 185, 185, 123, 123,
185, 185, 123, 123, 185, 185, 110, 110, 164, 164, 110, 110, 164, 164, 110, 110, 164, 164, 110, 110, 164, 164, 110, 110, 164, 164,
110, 110, 164, 164, 110, 110, 164, 164, 110, 110, 164, 164};

//-----------------------------------------------------------------------------------------------------------------------

//Pokemon Fight----------------------------------------------------------------------------------------------------------
//static volatile uint16_t notes4[]=


//Array of the songs for the radio-----------------------------------------------------------------------------------------------------------
static volatile uint32_t radio[] = {notes,notes2,notes3, notes4};
static volatile uint16_t radioSizes[] = {sizeof(notes), sizeof(notes2), sizeof(notes3), sizeof(notes4)};
static uint8_t station = 0;
static volatile uint32_t timer15Cnt = 0;
//-----------------------------------------------------------------------------------------------------------------------



void TIM1_BRK_TIM15_IRQHandler (void){

    timer15Cnt += TIM15->ARR / 256;

    if(station == 255){
        //srand(f);
        //setFreq(20 + rand() % 4000);
        //f++;
    }
    else{
        uint16_t* a = radio[station];
        uint16_t s = radioSizes[station];
        f = ++f % (s / sizeof(uint16_t));
        setFreq(a[f]);
    }
    TIM15->SR &= ~0x0001; //Clear interrupt bit


}

uint32_t getTimer15Cnt(){
    return timer15Cnt;
}

void resetTimer15Cnt(){
    timer15Cnt = 0;
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

//return 16.16 format mapped from 4096 to fixed point allmost 256
uint32_t convert(uint32_t val)
{
    return val << 12;
}

uint32_t remap(uint32_t low1, uint32_t high1, uint32_t low2, uint32_t high2, uint32_t val)
{

    float f = (float)(high2 - low2) / (float)(high1 - low1);

    return low2 + (val - low1) * f;
    //return (low2 << 16) + divideFix( multiplyFix((val - (low1 << 16)), (high2 - low2) << 16), (high1 - low1) << 16);
    //return (low2 << 16) + ((val - (low1 << 16)) * (high2 - low2) << 16) / (high1 - low1) << 16;
}

void sprintFixed(uint32_t i, char* str){
 // Prints a signed 16.16 fixed point number
    if ((i & 0x80000000) != 0) { // Handle negative numbers
        sprintf(str, "-");
        i = ~i + 1;
    }
    sprintf(str,"%d.%02d", i >> 16, 10000 * (uint32_t)(i & 0xFFFF) >> 16);
    // Print a maximum of 4 decimal digits to avoid overflow
}



void setupRadio(char* toPrint){
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN; // Enable clock line for GPIO bank B
    GPIOB->MODER &= ~(0x00000003 << (10 * 2)); // Clear mode register
    GPIOB->MODER |= (0x00000002 << (10 * 2)); // Set mode register
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_1);

    io_config();

    //setupTimer(TIM2, RCC_APB1ENR_TIM2EN, 1000, 4);
    //configT15(310);
    configT2();
    configCount();

    configT15(240);
    setFreq(100);

    //potmetersjov-----------------------------------------------------
    //readAdc2();

    //LCD--------------------------------------------------------------
    lcd_init();
    io_config();

}


//used every frame, moved to global for speed up
static char freqstr[30];

void battleMusic() {
    configT15(550);
    station = 3;
}

void updateRadio(char* toPrint, uint8_t* buf){

    uint32_t freq = remap(0, 256 << 16, 87 << 16, 108 << 16, convert(readAdc1())); // convert(readAdc1()); //remap(0, 256, 40, 2000, convert(readAdc1()));
    uint32_t vol = remap(0, 256 << 16, 0, 100 << 16, convert(readAdc2())); //convert(readAdc2()); //remap(0, 256, 0, 100, convert(readAdc2()));

    sprintFixed(freq, freqstr);

    sprintf(toPrint,"FM: %s MHz", freqstr);
    lcd_write_string(buf, toPrint, 0,0, 0);
    sprintf(toPrint,"Vol: %d sound unit", (vol >> 16));
    lcd_write_string(buf, toPrint, 0,1, 0);
    lcd_push_buffer(buf);

    if (90 << 16 > freq && freq > 88 << 16)
    {
        configT15(240);
        station = 0;
    }

    else if(94 << 16 > freq && freq > 92 << 16)
    {
        configT15(240);
        station = 1;
    }
    else if(98 << 16 > freq && freq > 96 << 16)
    {
        configT15(310);
        station = 2;
    }

    else
    {
        //configT15();
        setFreq(2000);
        station = 255;
    }
}
