# Paths must be relative on Windows.
ARDMK_DIR = ../mk
ARDUINO_DIR = ../../../../../lib/arduino-1.6.6
ARDUINO_LIBS = MeggyJrRGB
USER_LIB_PATH = ../libraries

AVR_TOOLS_DIR = $(ARDUINO_DIR)/hardware/tools/avr
LDFLAGS += -L$(AVR_TOOLS_DIR)/lib

BOARD_TAG = diecimila
BOARD_SUB = atmega328
MONITOR_PORT = com3

include $(ARDMK_DIR)/Arduino.mk

# Override device path for mintty.
DEVICE_PATH = com3
