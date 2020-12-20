###############################################################################
#
#                                 Makefile
#
#
#   Dieses Makefile muss folgende benutzerdefinierte Angaben enthalten:
#
#   PROJECT
#        Der Name des Hauptprogramms (welches u.a. die main - Funktion
#        beinhaltet OHNE jegliche Erweiterung
#
#   SRCS
#        hier sind die zu erzeugenden Dateien anzugeben (Hauptprogramm
#        welches die Funktion Main enthaellt plus zusaetzliche Software-
#        module ohne weiteren "Trenner":
#
#        SRCS    = ../src/serial_demo.o
#        SRCS   += ../src/readint.o
#
#        Die Dateien serial_demo.c und readint.c werden zu Objektdateien
#        uebersetzt und dem Gesamtprogramm hinzugelinkt
#
#
#   NCURSES
#        = 1 NCURSES Bibliothek wird eingebunden
#        = 0 wird nicht eingebunden
#
#   MATH
#        = 1 MATH Bibliothek (libm) wird eingebunden
#        = 0 wird nicht eingebunden
#
#   Aufrufoption des Makefiles:
#        make oder make all : erstellt alle .o und das ausfuehrbare Programm
#        make compile       : compiliert nur das Hauptprogramm ohne weitere
#                             Bibliotheken
#        make clean         : loescht alle erstellten Dateien (sinnvoll,
#                             wenn Softwaremodule / Bibliotheken veraendert
#                             wurden)
#
#
#   August 2017,  R. Seelig
#
###############################################################################

CC      = gcc
LD      = ld


OBJECTS   = ./$(PROJECT).o $(SRCS)

CC_FLAGS = -c -Os -std=gnu99

CC_SYMBOLS =
LD_FLAGS   = $(ADD_LIBS)

ifeq ($(NCURSES), 1)
  LD_FLAGS   += -lncurses -lpanel -lmenu
endif

ifeq ($(MATH), 1)
  LD_FLAGS += -lm
endif

ifeq ($(SDL), 1)
  LD_FLAGS += -lSDL -lSDL_image
endif


.PHONY: all clean compile flash

all: clean $(PROJECT)

compile:
	$(CC) $(PROJECT).c $(CC_FLAGS) $(CC_SYMBOLS) $(INCLUDE_PATHS) -o $(PROJECT).o

clean:
	rm -f $(PROJECT).o $(PROJECT)
	rm -f $(SRCS)

.c.o:
	$(CC) $< $(CC_FLAGS) $(CC_SYMBOLS) $(INCLUDE_PATHS) -o  $@


$(PROJECT): $(OBJECTS)
	$(CC) $(LD_FLAGS) $^ -Os -o $@

flash:
	@echo "PC / Raspi can't be flashed"
