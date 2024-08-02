#include "../inc/can_protocol.h"
#include "../inc/uart.h"
#include <string.h>
#include <util/crc16.h>

#define CAN_START_BYTE 0xAA
#define CAN_END_BYTE 0x55
#define CAN_EXTENDED_ID_FLAG 0x80

void can_protocol_init(void) {
    uart_init(9600);  // Initialize UART with appropriate baud rate
}

bool can_send_message(const can_message_t* msg) {
    uint8_t buffer[16];  // Increased buffer size to accommodate extended ID
    uint8_t index = 0;

    buffer[index++] = CAN_START_BYTE;

    // ID field (4 bytes for extended, 2 bytes for standard)
    if (msg->is_extended) {
        buffer[index++] = CAN_EXTENDED_ID_FLAG | ((msg->id >> 24) & 0x1F);
        buffer[index++] = (msg->id >> 16) & 0xFF;
        buffer[index++] = (msg->id >> 8) & 0xFF;
        buffer[index++] = msg->id & 0xFF;
    } else {
        buffer[index++] = (msg->id >> 8) & 0x07;
        buffer[index++] = msg->id & 0xFF;
    }

    buffer[index++] = msg->dlc & 0x0F;

    for (uint8_t i = 0; i < msg->dlc && i < CAN_MAX_DATA_LENGTH; i++) {
        buffer[index++] = msg->data[i];
    }

    uint16_t crc = can_calculate_crc(msg);
    buffer[index++] = (crc >> 8) & 0xFF;
    buffer[index++] = crc & 0xFF;

    buffer[index++] = CAN_END_BYTE;

    for (uint8_t i = 0; i < index; i++) {
        uart_transmit(buffer[i]);
    }

    return true;
}

bool can_receive_message(can_message_t* msg, uint16_t timeout_ms) {
    uint8_t buffer[16];
    uint8_t index = 0;
    uint16_t timer = 0;

    while (uart_receive() != CAN_START_BYTE) {
        if (++timer >= timeout_ms) return false;
    }

    buffer[index++] = CAN_START_BYTE;

    while (index < sizeof(buffer)) {
        if (uart_receive_available()) {
            buffer[index] = uart_receive();
            if (buffer[index] == CAN_END_BYTE) {
                index++;
                break;
            }
            index++;
        } else {
            if (++timer >= timeout_ms) return false;
        }
    }

    if (index < 7 || buffer[index - 1] != CAN_END_BYTE) return false;

    index = 1;  // Skip start byte

    if (buffer[index] & CAN_EXTENDED_ID_FLAG) {
        msg->is_extended = true;
        msg->id = ((uint32_t)(buffer[index] & 0x1F) << 24) |
                  ((uint32_t)buffer[index + 1] << 16) |
                  ((uint32_t)buffer[index + 2] << 8) |
                  buffer[index + 3];
        index += 4;
    } else {
        msg->is_extended = false;
        msg->id = ((uint16_t)(buffer[index] & 0x07) << 8) | buffer[index + 1];
        index += 2;
    }

    msg->dlc = buffer[index++] & 0x0F;

    for (uint8_t i = 0; i < msg->dlc && i < CAN_MAX_DATA_LENGTH; i++) {
        msg->data[i] = buffer[index++];
    }

    uint16_t received_crc = ((uint16_t)buffer[index] << 8) | buffer[index + 1];
    uint16_t calculated_crc = can_calculate_crc(msg);

    return (received_crc == calculated_crc);
}

uint16_t can_calculate_crc(const can_message_t* msg) {
    uint16_t crc = 0xFFFF;

    crc = _crc16_update(crc, msg->is_extended ? CAN_EXTENDED_ID_FLAG : 0);
    crc = _crc16_update(crc, (msg->id >> 24) & 0xFF);
    crc = _crc16_update(crc, (msg->id >> 16) & 0xFF);
    crc = _crc16_update(crc, (msg->id >> 8) & 0xFF);
    crc = _crc16_update(crc, msg->id & 0xFF);
    crc = _crc16_update(crc, msg->dlc);

    for (uint8_t i = 0; i < msg->dlc && i < CAN_MAX_DATA_LENGTH; i++) {
        crc = _crc16_update(crc, msg->data[i]);
    }

    return crc;
}

int can_arbitrate(const can_message_t* msg1, const can_message_t* msg2) {
    if (msg1->is_extended != msg2->is_extended) {
        return msg1->is_extended ? 1 : -1;  // Standard ID has higher priority
    }

    uint32_t mask = msg1->is_extended ? 0x1FFFFFFF : 0x7FF;
    uint32_t id1 = msg1->id & mask;
    uint32_t id2 = msg2->id & mask;

    if (id1 < id2) return -1;
    if (id1 > id2) return 1;
    return 0;  // IDs are equal
}
