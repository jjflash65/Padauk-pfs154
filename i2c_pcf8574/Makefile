############################################################
#
#                         Makefile
#
############################################################

PROJECT       = pcf8574_readtest
MCU           = PFS154
MEMORG        = pdk14
F_CPU         = 8000000
FACTORYCAL    = 1

# hier alle zusaetzlichen Softwaremodule angegeben
SRCS          = ../src/delay.rel
SRCS         += ../src/i2c.rel
SRCS         += ../src/uart.rel
SRCS         += ../src/my_printf.rel

INC_DIR       = -I./ -I../include

# benutzbare Programmer:
#  1 : easypdkprogrammer  ==> serielle Portangabe kann frei bleiben
#  2 : pfsprog            ==> benoetigt serielle Portangabe

PROGRAMMER    = 2
SERPORT       = /dev/ttyUSB0
CH340RESET    = 1

include ../makefile.mk
