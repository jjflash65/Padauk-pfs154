############################################################
#
#                         Makefile
#
############################################################

PROJECT       = ch340reset

# hier alle zusaetzlichen Softwaremodule angegeben
SRCS          = device.o
SRCS         += interface.o
SRCS         += endpoint.o
SRCS         += raw.o


INCLUDE_PATHS = -I./

MATH          = 0
NCURSES       = 0
SDL           = 0

ADD_LIBS      = -ludev


include ./makefile.mk
