#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"
#include "can_protocol.h"

#define BAUD_RATE 9600
#define LED_PIN PB5
#define RECEIVE_TIMEOUT_MS 1000

void initialize_hardware(void) {
    // Set LED pin as output
    DDRB |= (1 << LED_PIN);
    
    // Initialize UART
    uart_init(BAUD_RATE);
    
    // Initialize CAN protocol
    can_protocol_init();
}

void toggle_led(void) {
    PORTB ^= (1 << LED_PIN);
}

int main(void) {
    initialize_hardware();
    
    can_message_t received_msg;
    uint32_t last_message_count = 0;
    
    while (1) {
        if (can_receive_message(&received_msg, RECEIVE_TIMEOUT_MS)) {
            // Message received successfully
            if (received_msg.id == 0x123 && received_msg.dlc == 4) {
                // Extract message count from data (little-endian)
                uint32_t message_count = ((uint32_t)received_msg.data[3] << 24) |
                                         ((uint32_t)received_msg.data[2] << 16) |
                                         ((uint32_t)received_msg.data[1] << 8) |
                                         (uint32_t)received_msg.data[0];
                
                // Check if this is a new message
                if (message_count != last_message_count) {
                    // Toggle LED to indicate new message received
                    toggle_led();
                    last_message_count = message_count;
                }
            }
        }
    }
    
    return 0;
}
