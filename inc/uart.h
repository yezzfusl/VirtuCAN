#ifndef UART_H
#define UART_H

#include <stdint.h>

// UART initialization
void uart_init(uint32_t baud_rate);

// UART transmit single byte
void uart_transmit(uint8_t data);

// UART receive single byte
uint8_t uart_receive(void);

// UART transmit string
void uart_transmit_string(const char* str);

// UART receive string (with timeout)
uint8_t uart_receive_string(char* buffer, uint8_t max_length, uint16_t timeout_ms);

#endif // UART_H
