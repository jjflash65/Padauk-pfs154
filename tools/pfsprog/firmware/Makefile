############################################################
#
#                         Makefile
#
############################################################

PROJECT    = pfs154_prog2_1


INC_DIR    = -I./ -I../include

# hier alle zusaetzlichen Softwaremodule angegeben

SRCS       = ./my_printf.o
SRCS      += ./uart_all.o

PRINT_FL   = 0
SCAN_FL    = 0
MATH       = 0

# fuer Compiler / Linker
FREQ       = 16000000ul
MCU        = atmega328p

# fuer AVRDUDE
PROGRAMMER = arduino
PROGPORT   = /dev/ttyUSB0
BRATE      = 115200
DUDEOPTS   = -B1

include ./makefile.mk

