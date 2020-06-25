#ifndef _30010_IO_H_
#define _30010_IO_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f30x_conf.h"
#include <stdio.h>

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define UART_BUFFER_LENGTH 256

/* Exported functions ------------------------------------------------------- */
/****************************/
/*** USB Serial Functions ***/
/****************************/
void uart_init(uint32_t baud);
void uart_put_char(uint8_t c);
uint8_t uart_get_char();
uint8_t uart_get_count();
void uart_clear();

/*****************************/
/*** LCD Control Functions ***/
/*****************************/
void lcd_init();
void lcd_transmit_byte(uint8_t data);
void lcd_push_buffer(uint8_t* buffer);
void lcd_write_string(uint8_t* buf, const char* toPrint, uint8_t x, uint8_t y, uint8_t inverse);
void lcd_graphics_buffer(uint8_t* buf, uint16_t bufsize);
void lcd_shift_right(uint8_t* buf, uint8_t* shiftBuf, int16_t offset);
void lcd_reset();
void io_config();

#endif /* _30010_IO_H_ */