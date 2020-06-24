
#include "stm32f30x_conf.h"
#include <stdlib.h>
#include "30010_io.h"
#include "Utility.h"
#include <string.h>

void updateRadio(char* toPrint, uint8_t* buf);
void setupRadio(char* toPrint);
uint32_t getTimer15Cnt();
void resetTimer15Cnt();
void battleMusic();
