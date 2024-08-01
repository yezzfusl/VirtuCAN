#include "../inc/uart.h"
#include <avr/io.h>
#include <util/delay.h>

#define BAUD_PRESCALE(baud) ((F_CPU / 16 / baud) - 1)

void uart_init(uint32_t baud_rate) {
    uint16_t baud_prescale = BAUD_PRESCALE(baud_rate);
    
    // Set baud rate
    UBRR0H = (uint8_t)(baud_prescale >> 8);
    UBRR0L = (uint8_t)(baud_prescale);
    
    // Enable receiver and transmitter
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    
    // Set frame format: 8 data bits, 1 stop bit, no parity
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_transmit(uint8_t data) {
    // Wait for empty transmit buffer
    while (!(UCSR0A & (1 << UDRE0)));
    
    // Put data into buffer, sends the data
    UDR0 = data;
}

uint8_t uart_receive(void) {
    // Wait for data to be received
    while (!(UCSR0A & (1 << RXC0)));
    
    // Get and return received data from buffer
    return UDR0;
}

void uart_transmit_string(const char* str) {
    while (*str) {
        uart_transmit(*str++);
    }
}

uint8_t uart_receive_string(char* buffer, uint8_t max_length, uint16_t timeout_ms) {
    uint8_t i = 0;
    uint16_t timer = 0;
    
    while (i < max_length - 1) {
        if (UCSR0A & (1 << RXC0)) {
            buffer[i] = UDR0;
            if (buffer[i] == '\n' || buffer[i] == '\r') {
                break;
            }
            i++;
            timer = 0;
        } else {
            _delay_ms(1);
            timer++;
            if (timer >= timeout_ms) {
                break;
            }
        }
    }
    
    buffer[i] = '\0';
    return i;
}
