/*--------------------------------------------------------
                              blink_int.c

     Blinkprogramm mittels interruptgesteuertem Timer
     realisiert

     Compiler  : SDCC 4.0.3
     MCU       : PFS154 / PFS173

     07.10.2020        R. Seelig

  -------------------------------------------------------- */

#include <stdint.h>
#include "pdk_init.h"
#include "pfs1xx_gpio.h"

// LED-Anschlusspin
#define led_init()     PA0_output_init()
#define led_set()      PA0_set()
#define led_clr()      PA0_clr()
#define led_toggle()   PA0_toggle()


/* --------------------------------------------------------
                            t16_init

     initialisiert den 16-Bit Timer mit Interruptaus-
     loesung.

     Das Modusregister T16M beinhaltet die Konfiguration
     des Timers hinsichtlich der Taktquelle, des Teilers
     und des Ausloesen eines Interrupts.

     T16M
     ------------------------------------------------------
     Bit 7:5      |  000 Timer disable
                  |  001 CLK
                  |  010 reserved
                  |  011 fallende Taktflanke an PA4
     Taktquelle   |  100 int. Hi-Speed RC-Oszillator (IHRC)
                  |  101 ext. Oszillator
                  |  110 int. Lo-Speed RC-Oszillator (ILRC)
                  |  111 fallende Taktflanke an PA0
     ------------------------------------------------------
     Bit 4:3      |   00  / 1
                  |   01  / 4
     Taktteiler   |   10  / 16
                  |   11  / 64
     ------------------------------------------------------
                  |   Interruptquelle. Eine Aenderung des
                  |   eines Bits im Timerregister T16C
                  |   loest einen Interrupt aus (faktisch
                  |   ein weiterer Teiler)
                  |----------------------------------------
                  |   000 Bit  8  ==> Teiler / 256
     Bit 2:0      |   001 Bit  9
                  |   010 Bit 10
                  |   011 Bit 11
                  |   100 Bit 12
                  |   101 Bit 13
                  |   110 Bit 14
                  |   111 Bit 15  ==> Teiler / 32768

     Durch den Taktvorteiler und das Einstellen der Inter-
     ruptquellen sind somit Gesamtteilerverhaeltnisse von
     Taktquelle / 1 / 256 bis Taktquelle / 64 / 32768
   -------------------------------------------------------- */
void t16_init(void)
{
  T16M = (uint8_t)(T16M_CLK_IHRC | T16M_CLK_DIV64 | T16M_INTSRC_15BIT);

  __engint();                   // grundsaetzlich Interrupt zulassen
  INTEN |= INTEN_T16;           // Timerinterrupt zulassen
}

/* --------------------------------------------------------
                       interrupt

     der Interrupt-Handler. Innerhalb dieser Funktion
     muss bei mehreren Interruptquellen das Interrupt-
     request Register INTRQ ausgewertet werden.

     Der Interrupthandler zaehlt die Variable millis
     hoch und quittiert die Interruptanforderung.

     ==> Intervallzeit * 32 = 32 us * 32 = 10224 us
     ==> hierdurch ergibt sich eine Zeit von 1,024 ms
   -------------------------------------------------------- */
void interrupt(void) __interrupt(0)
{

  if (INTRQ & INTRQ_T16)
  {
    // LED blinken geschieht ausschliesslich im Interrupthandler
    led_toggle();

    INTRQ &= ~INTRQ_T16;          // Interruptanforderung quittieren
  }
}

/* --------------------------------------------------
                           main
   -------------------------------------------------- */
void main(void)
{
  led_init();
  t16_init();

  while(1);
}
