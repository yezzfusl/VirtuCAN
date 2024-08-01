#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"
#include "can_protocol.h"

#define BAUD_RATE 9600
#define LED_PIN PB5
#define BUTTON_PIN PD2

void initialize_hardware(void) {
    // Set LED pin as output
    DDRB |= (1 << LED_PIN);
    
    // Enable pull-up resistor on button pin
    PORTD |= (1 << BUTTON_PIN);
    
    // Initialize UART
    uart_init(BAUD_RATE);
    
    // Initialize CAN protocol
    can_protocol_init();
}

uint8_t is_button_pressed(void) {
    return !(PIND & (1 << BUTTON_PIN));
}

void toggle_led(void) {
    PORTB ^= (1 << LED_PIN);
}

int main(void) {
    initialize_hardware();
    
    can_message_t msg;
    uint32_t message_count = 0;
    
    while (1) {
        if (is_button_pressed()) {
            // Debounce
            _delay_ms(50);
            
            if (is_button_pressed()) {
                // Prepare CAN message
                msg.id = 0x123;  // Example ID
                msg.dlc = 4;     // 4 bytes of data
                
                // Fill data with message count (little-endian)
                msg.data[0] = message_count & 0xFF;
                msg.data[1] = (message_count >> 8) & 0xFF;
                msg.data[2] = (message_count >> 16) & 0xFF;
                msg.data[3] = (message_count >> 24) & 0xFF;
                
                // Send CAN message
                can_send_message(&msg);
                
                // Toggle LED to indicate message sent
                toggle_led();
                
                // Increment message count
                message_count++;
                
                // Wait for button release
                while (is_button_pressed()) {
                    _delay_ms(10);
                }
            }
        }
    }
    
    return 0;
}
