############################################################
#
#                         Makefile
#
############################################################

PROJECT       = charlie6_demo
MCU           = PFS154
MEMORG        = pdk14
F_CPU         = 8000000
FACTORYCAL    = 1

# hier alle zusaetzlichen Softwaremodule angegeben
SRCS          = ../src/delay.rel
SRCS         += ./charlie6.rel

INC_DIR       = -I./ -I../include

# benutzbare Programmer:
#  1 : easypdkprogrammer  ==> serielle Portangabe kann frei bleiben
#  2 : pfsprog            ==> benoetigt serielle Portangabe
#  3 : pfsprog            ==> wie (2), allerdings ohne Bargraphanzeige
#                             in der Konsole

PROGRAMMER    = 1
SERPORT       = /dev/ttyUSB0
#SERPORT       = /dev/ttyACM0
CH340RESET    = 0


include ../makefile.mk
