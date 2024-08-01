# VirtuCAN
A virtual CAN bus simulation over UART using AVR microcontrollers.

## Project Structure

- `Makefile`: Build system configuration
- `src/transmitter.c`: CAN transmitter implementation
- `src/receiver.c`: CAN receiver implementation
- `src/uart.c` and `inc/uart.h`: UART communication functions
- `src/can_protocol.c` and `inc/can_protocol.h`: CAN protocol implementation
## Prerequisites

Ensure you have the following tools installed:

- AVR-GCC
- AVRDUDE
- Make

## Building the Project

1. To build both the transmitter and receiver:
    - `make all`

2. To build only the transmitter:
    - `make transmitter.hex`

3. To build only the receiver:
    - `make receiver.hex`

## Uploading to Arduino Uno

1. To upload the transmitter code:
    - `make upload_transmitter`

2. To upload the receiver code:
    - `make upload_receiver`

## Note 📝: Ensure that the correct serial ports are specified in the Makefile for each Arduino board.

## Hardware Setup

1. Connect two Arduino Uno boards via their UART pins (TX to RX, RX to TX, and GND to GND).
2. On the transmitter Arduino:
   - Connect a pushbutton to digital pin 2 (PD2) and GND.
   - Connect an LED with a current-limiting resistor to digital pin 13 (PB5) and GND.
3. On the receiver Arduino:
   - Connect an LED with a current-limiting resistor to digital pin 13 (PB5) and GND.

## Usage

1. Upload the transmitter code to one Arduino and the receiver code to the other.
2. Press the button on the transmitter Arduino to send a CAN message.
3. The LED on the transmitter will toggle to indicate a message was sent.
4. If the message is successfully received, the LED on the receiver will toggle.

## Implementation Details

This project implements a simplified CAN-like protocol over UART with the following features:

- 29-bit message ID
- Up to 8 bytes of data per message
- CRC-16 for error detection
- Timeout-based reception
- Start and end bytes for message framing

The transmitter sends a message with an incrementing counter as data when the button is pressed. The receiver decodes the message and toggles its LED if a new message is received successfully.

## License

This project is open-source and available under the MIT License.
