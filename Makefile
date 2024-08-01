# Makefile for CAN over UART project

# MCU settings
MCU = atmega328p
F_CPU = 16000000UL

# Compiler settings
CC = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -Wextra -std=c99

# Project structure
SRC_DIR = src
INC_DIR = inc
BUILD_DIR = build

# Source files
UART_SRC = $(SRC_DIR)/uart.c
CAN_PROTOCOL_SRC = $(SRC_DIR)/can_protocol.c
TRANSMITTER_SRC = $(SRC_DIR)/transmitter.c
RECEIVER_SRC = $(SRC_DIR)/receiver.c

# Header files
UART_INC = $(INC_DIR)/uart.h
CAN_PROTOCOL_INC = $(INC_DIR)/can_protocol.h

# Targets
TRANSMITTER = $(BUILD_DIR)/transmitter.hex
RECEIVER = $(BUILD_DIR)/receiver.hex

# Default target
all: $(TRANSMITTER) $(RECEIVER)

# Compile transmitter
$(TRANSMITTER): $(UART_SRC) $(CAN_PROTOCOL_SRC) $(TRANSMITTER_SRC)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) $^ -o $(BUILD_DIR)/transmitter.elf
	$(OBJCOPY) -O ihex -R .eeprom $(BUILD_DIR)/transmitter.elf $@

# Compile receiver
$(RECEIVER): $(UART_SRC) $(CAN_PROTOCOL_SRC) $(RECEIVER_SRC)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) $^ -o $(BUILD_DIR)/receiver.elf
	$(OBJCOPY) -O ihex -R .eeprom $(BUILD_DIR)/receiver.elf $@

# Upload transmitter
upload_transmitter: $(TRANSMITTER)
	$(AVRDUDE) -p $(MCU) -c arduino -P /dev/ttyACM0 -U flash:w:$

# Upload receiver
upload_receiver: $(RECEIVER)
	$(AVRDUDE) -p $(MCU) -c arduino -P /dev/ttyACM1 -U flash:w:$

# Clean build files
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean upload_transmitter upload_receiver
