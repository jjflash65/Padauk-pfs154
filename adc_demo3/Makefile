############################################################
#
#                         Makefile
#
############################################################

PROJECT       = adc_demo_03
MCU           = PFS154
MEMORG        = pdk14
F_CPU         = 8000000
FACTORYCAL    = 1


# hier alle zusaetzlichen Softwaremodule angegeben
SRCS          = ../src/delay.rel
SRCS         += ../src/adc_pfs154.rel
SRCS         += ../src/seg7mpx_dig4.rel

INC_DIR       = -I./ -I../include

# benutzbare Programmer:
#  1 : easypdkprogrammer  ==> serielle Portangabe kann frei bleiben
#  2 : pfsprog            ==> benoetigt serielle Portangabe

PROGRAMMER    = 2
SERPORT       = /dev/ttyUSB0
CH340RESET    = 1


include ../makefile.mk
