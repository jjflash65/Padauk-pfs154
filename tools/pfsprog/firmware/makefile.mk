###############################################################################
#
#                                 makefile.mk
#
#   Funktionaler Teil eines Makefiles. Diese Datei wird von einem "regulaeren"
#   Makefile inkludiert.
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
#   PRINT_FL / SCAN_FL
#        = 1 wenn Unterstuetzung fuer Gleitkommazahlen mittels printf / scanf
#            vorhanden sein soll.
#        = 0 ohne unterstuetzung
#
#
#   INC_DIR
#        Suchverzeichnis, in dem zusaetzliche Programmmodule liegen
#
#
#   MCU
#        Name des zu verwendenden Mikrocontrollers
#
#
#   FREQ
#        Taktfrequenz des Controllers, die Angabe hier ist gleichbedeutend
#        einem
#                 #define F_CPU frequenz
#        im C-Programm
#
#
#   PROGRAMMER / BRATE / PROGPORT
#        gibt den Programmernamen, den Port und die Baudrate an, an den
#        ein Programmer angeschlossen ist.
#
#
#   Aufrufoption des Makefiles:
#        make oder make all : erstellt alle .o und Hexdateien
#        make compile       : compiliert nur das Hauptprogramm ohne weitere
#                             Bibliotheken
#        make clean         : loescht alle erstellten Dateien (sinnvoll,
#                             wenn Softwaremodule / Bibliotheken veraendert
#                             wurden)
#        make size          : zeigt die Groesse der erstellten Hex-Datei an
#        make flash         : flasht den Zielcontroller
#
#
#   August 2017,  R. Seelig
#
###############################################################################

CC        = avr-gcc
LD        = avr-gcc
OBJCOPY   = avr-objcopy
SIZE      = avr-size

ifeq ($(INC_DIR),)
	INC_DIR   := -I./ -I../include
endif

ifeq ($(PROGPORT),)
PROGPORT := /dev/ttyUSB0
endif


OBJECTS    = $(PROJECT).o $(SRCS)

CPU        = -mmcu=$(MCU)

#CC_FLAGS   = -Os $(CPU) -std=c99
CC_FLAGS   = -Os $(CPU)

CC_SYMBOLS = -DF_CPU=$(FREQ)
LD_FLAGS   = $(CPU)

ifeq ($(PRINTF_FL), 1)
  LD_FLAGS += -Wl,-u,vfprintf
endif

ifeq ($(SCANF_FL), 1)
  LD_FLAGS += -Wl,-u,vfscanf -lscanf_flt
endif

LD_SYS_LIB =

ifeq ($(PRINTF_FL), 1)
  LD_SYS_LIB += -lprintf_flt
endif

ifeq ($(SCANF_FL), 1)
  LD_SYS_LIB += -lscanf_flt
endif

ifeq ($(MATH), 1)
  LD_SYS_LIB +=-lm
endif


.PHONY: all clean size compile flash

all: clean $(PROJECT).hex size

compile:
	$(CC) $(CC_FLAGS) $(CC_SYMBOLS) $(INC_DIR) -o $(PROJECT).o $(PROJECT).c

clean:
	rm -f $(PROJECT).o $(PROJECT).elf $(PROJECT).hex $(PROJECT).map
	rm -f $(SRCS)

.c.o:
	$(CC) $< -c -o $@ $(CC_FLAGS) $(CC_SYMBOLS) $(INC_DIR)

.S.o:
	$(CC) $< -c -o $@ $(CC_FLAGS) $(CC_SYMBOLS) $(INC_DIR)

$(PROJECT).elf: $(OBJECTS)
	$(LD) $(LD_FLAGS) $^$(LD_SYS_LIB) -o $@

$(PROJECT).hex: $(PROJECT).elf
	@$(OBJCOPY) -j .text -j .data -O ihex $< $@

$(PROJECT).lst: $(PROJECT).elf
	@$(OBJDUMP) -Sdh $< > $@

size: $(PROJECT).elf
	$(SIZE) -C $(PROJECT).elf --mcu=$(MCU) 1>&2

flash:
ifeq ($(CH340RESET), 1)
	ch340reset
	sleep 1
endif

ifeq ($(PROGRAMMER), usbtiny)
	avrdude -c $(PROGRAMMER) -p $(MCU) $(DUDEOPTS) -V -U flash:w:$(PROJECT).hex
endif
ifeq ($(PROGRAMMER), usbasp)
	avrdude -c $(PROGRAMMER) -p $(MCU) $(DUDEOPTS) -V -U flash:w:$(PROJECT).hex
endif

ifeq ($(PROGRAMMER), avrisp)
	avrdude -c $(PROGRAMMER) -p $(MCU) -P $(PROGPORT) -b $(BRATE) $(DUDEOPTS) -V -U flash:w:$(PROJECT).hex
endif

ifeq ($(PROGRAMMER), wiring)
	avrdude -c $(PROGRAMMER) -p $(MCU) -P $(PROGPORT) -b $(BRATE) $(DUDEOPTS) -V -D -U flash:w:$(PROJECT).hex
endif

ifeq ($(PROGRAMMER), stk500v2)
	avrdude -c $(PROGRAMMER) -p $(MCU) -P $(PROGPORT) -b $(BRATE) $(DUDEOPTS) -V -U flash:w:$(PROJECT).hex
endif

ifeq ($(PROGRAMMER), ponyser)
	avrdude -c $(PROGRAMMER) -p $(MCU) -P $(PROGPORT) $(DUDEOPTS) -V -U flash:w:$(PROJECT).hex
endif
ifeq ($(PROGRAMMER), arduino)
	avrdude -c $(PROGRAMMER) -p $(MCU) -P $(PROGPORT) -b $(BRATE) $(DUDEOPTS) -V -U flash:w:$(PROJECT).hex

endif
