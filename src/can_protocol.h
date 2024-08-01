#ifndef CAN_PROTOCOL_H
#define CAN_PROTOCOL_H

#include <stdint.h>

// CAN message structure
typedef struct {
    uint32_t id;
    uint8_t dlc;
    uint8_t data[8];
} can_message_t;

// CAN protocol initialization
void can_protocol_init(void);

// Send CAN message
void can_send_message(const can_message_t* msg);

// Receive CAN message (with timeout)
uint8_t can_receive_message(can_message_t* msg, uint16_t timeout_ms);

// Calculate CRC for CAN message
uint16_t can_calculate_crc(const can_message_t* msg);

#endif // CAN_PROTOCOL_H
