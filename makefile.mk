# ---------------------------------------------------------------
#                           makefile.mk
#
#    gemeinsames Makefile fuer alle PFS154-Projekte, wird von
#    den entsprechenden Makefiles der Projekte includiert
#
#    Die Makefiles der Projekte enthalten lediglich den
#    Quelldateinamen des Programms aus dem ein auf dem
#    STM8 lauffaehiges Programm generiert werden soll
#
#    Oktober 2020    R. Seelig
#
# ---------------------------------------------------------------

# Targetspannung in mV
VDD_MV        = 5000


# Pfad zu Include Dateien
ifeq ($(INC_DIR),)
    INC_DIR  := -I../include/
endif

INC_DIR      += -I../tools/include

# Compilersymbole (defines)
CC_SYMBOLS   =

# Pfad zur Toolchain (Compiler, Linker, Assembler)
TOOLPATH     = ../tools/bin/

# -----------------------------------------------------------------------------------------------------
#  hier endet das "User-Interface des Makefiles und es sollte  ab hier nur mit Bedacht Aenderungen
#  vorgenommen werden !!!
# -----------------------------------------------------------------------------------------------------

CC           = $(TOOLPATH)sdcc
LD           = $(TOOLPATH)sdld
OBJCOPY      = $(TOOLPATH)sdobjcopy
SIZEPROG     = @../tools/bin/pfsreadhex


# -----------------------------------------------------------------------------------------------------
# bei fehlenden Angaben Defaultwerte setzen
# -----------------------------------------------------------------------------------------------------

# Assembler fuer 14 oder 15 Bit Architektur
ifeq ($(MEMORG), pdk14)
	AS   = $(TOOLPATH)sdaspdk14
endif

ifeq ($(MEMORG), pdk15)
	AS   = $(TOOLPATH)/sdaspdk15
endif

ifeq ($(FACTORYCAL),)
	FACTORYCAL = 1
endif

ifeq ($(PROGRAMMER),)
	PROGRAMMER = 1
endif

ifeq ($(SERPORT),)
	SERPORT = /dev/ttyUSB0
endif

# Hier angeben, ob ein AVR basierender Programmer einen Bootloader beinhaltet
# (bei einem vorhandenen Bootloader muss eine Wartezeit eingelegt werden, weil bei
# einer Verbindung zum AVR ein Reset ausgefuehrt wird und die Wartezeit des Boot-
# loaders abgewartet werden muss

NOWAIT       = nowait

#NOWAIT       =

# Speicherorganisation des Padauk-Controllers
LIBSPEC      = -m$(MEMORG)


OBJS         = $(PROJECT).rel $(SRCS)

CC_FLAGS     = -m$(MEMORG) -D$(MCU) -DF_CPU=$(F_CPU) -DFACTORYCAL=$(FACTORYCAL)
CC_FLAGS    += --std-sdcc11 --opt-code-size


.PHONY: all compile clean flash complete run

all: clean $(OBJS)
	@echo "Linking $(PROJECT).c with libs, Intel-Hex-File: $(PROJECT).ihx" 1>&2
	$(CC) $(LIBSPEC) $(INC_DIR) --out-fmt-ihx $(OBJS) 1>&2
#	$(OBJCOPY) -I ihex -O binary $(PROJECT).ihx $(PROJECT).bin	
	@rm -f ../src/*.asm
	@rm -f ../src/*.rst
	@rm -f ../src/*.rel
	@rm -f ../src/*.sym
	@rm -f ../src/*.lst
	@rm -f ../src/*.map
	@rm -f ../src/*.cdb
	@rm -f ../src/*.lk
	@rm -f ../src/*.mem
	@echo "  " 1>&2
	$(SIZEPROG) $(MCU) $(PROJECT) 1>&2
	@echo "  " 1>&2
	@echo " ------ Programm build sucessfull -----" 1>&2

compile:
	$(CC) -c $(CC_FLAGS) $(CC_SYMBOLS) $(INC_DIR) $(PROJECT).c -o $(PROJECT).rel 1>&2

clean:
	@rm -f *.asm
	@rm -f *.rst
	@rm -f *.ihx
	@rm -f *.rel
	@rm -f *.sym
	@rm -f *.lst
	@rm -f *.map
	@rm -f *.cdb
	@rm -f *.lk
	@rm -f *.mem
	@rm -f *.bin
	@rm -f ../src/*.asm
	@rm -f ../src/*.rst
	@rm -f ../src/*.rel
	@rm -f ../src/*.sym
	@rm -f ../src/*.lst
	@rm -f ../src/*.map
	@rm -f ../src/*.cdb
	@rm -f ../src/*.lk
	@rm -f ../src/*.mem

	@echo "Cleaning done..."

%.rel: %.c
	$(CC) -c $(CC_FLAGS) $(CC_SYMBOLS) $(INC_DIR) $< -o $@ 1>&2

flash:

ifeq ($(PROGRAMMER),1)
	../tools/bin/easypdkprog -n $(MCU) write $(PROJECT).ihx 1>&2
endif

ifeq ($(PROGRAMMER),2)

ifeq ($(CH340RESET),1)
	../tools/bin/ch340reset
#	sleep 0.5
endif

	../tools/bin/pfsprog txwr $(SERPORT) $(PROJECT).ihx $(NOWAIT) 1>&2
endif

run:
ifeq ($(PROGRAMMER),1)
	../tools/bin/easypdkprog start
endif
	
ifeq ($(PROGRAMMER),2)

ifeq ($(CH340RESET),1)
	../tools/bin/ch340reset
#	sleep 0.5
endif
	../tools/bin/pfsprog run $(SERPORT) $(NOWAIT) 1>&2
endif

stop:
ifeq ($(PROGRAMMER),2)

ifeq ($(CH340RESET),1)
	../tools/bin/ch340reset
#	sleep 0.5
endif
	../tools/bin/pfsprog stop $(SERPORT) $(NOWAIT) 1>&2
endif

complete: clean all flash
