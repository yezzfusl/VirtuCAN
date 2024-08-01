#include "can_protocol.h"
#include "uart.h"
#include <string.h>
#include <util/crc16.h>

#define CAN_START_BYTE 0xAA
#define CAN_END_BYTE 0x55

void can_protocol_init(void) {
}

void can_send_message(const can_message_t* msg) {
    uint8_t buffer[15]; // Start byte + ID (4) + DLC (1) + Data (8) + CRC (2) + End byte
    uint8_t index = 0;

    buffer[index++] = CAN_START_BYTE;

    // ID (32-bit, big endian)
    buffer[index++] = (msg->id >> 24) & 0xFF;
    buffer[index++] = (msg->id >> 16) & 0xFF;
    buffer[index++] = (msg->id >> 8) & 0xFF;
    buffer[index++] = msg->id & 0xFF;

    // DLC
    buffer[index++] = msg->dlc & 0x0F; // Ensure DLC is 0-8

    // Data
    for (uint8_t i = 0; i < msg->dlc; i++) {
        buffer[index++] = msg->data[i];
    }

    // Calculate CRC
    uint16_t crc = can_calculate_crc(msg);
    buffer[index++] = (crc >> 8) & 0xFF;
    buffer[index++] = crc & 0xFF;

    buffer[index++] = CAN_END_BYTE;

    // Transmit the entire message
    for (uint8_t i = 0; i < index; i++) {
        uart_transmit(buffer[i]);
    }
}

uint8_t can_receive_message(can_message_t* msg, uint16_t timeout_ms) {
    uint8_t buffer[15];
    uint8_t index = 0;
    uint16_t timer = 0;

    // Wait for start byte
    while (uart_receive() != CAN_START_BYTE) {
        if (++timer >= timeout_ms) {
            return 0; // Timeout
        }
    }

    buffer[index++] = CAN_START_BYTE;

    // Receive the rest of the message
    while (index < sizeof(buffer)) {
        if (UCSR0A & (1 << RXC0)) {
            buffer[index] = UDR0;
            if (buffer[index] == CAN_END_BYTE) {
                index++;
                break;
            }
            index++;
            timer = 0;
        } else {
            if (++timer >= timeout_ms) {
                return 0; // Timeout
            }
        }
    }

    if (index != sizeof(buffer) || buffer[index - 1] != CAN_END_BYTE) {
        return 0; // Invalid message
    }

    // Parse the message
    index = 1; // Skip start byte
    msg->id = ((uint32_t)buffer[index++] << 24) |
              ((uint32_t)buffer[index++] << 16) |
              ((uint32_t)buffer[index++] << 8) |
              (uint32_t)buffer[index++];

    msg->dlc = buffer[index++] & 0x0F;

    for (uint8_t i = 0; i < msg->dlc; i++) {
        msg->data[i] = buffer[index++];
    }

    // Verify CRC
    uint16_t received_crc = ((uint16_t)buffer[index++] << 8) | buffer[index++];
    uint16_t calculated_crc = can_calculate_crc(msg);

    return (received_crc == calculated_crc) ? 1 : 0;
}

uint16_t can_calculate_crc(const can_message_t* msg) {
    uint16_t crc = 0xFFFF;

    crc = _crc16_update(crc, (msg->id >> 24) & 0xFF);
    crc = _crc16_update(crc, (msg->id >> 16) & 0xFF);
    crc = _crc16_update(crc, (msg->id >> 8) & 0xFF);
    crc = _crc16_update(crc, msg->id & 0xFF);
    crc = _crc16_update(crc, msg->dlc);

    for (uint8_t i = 0; i < msg->dlc; i++) {
        crc = _crc16_update(crc, msg->data[i]);
    }

    return crc;
}
