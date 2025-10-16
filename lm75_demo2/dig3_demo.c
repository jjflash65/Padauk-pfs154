/*--------------------------------------------------------
                        dig3_demo.c

     Demo fuer eine 4-stellige gemultiplexte
     7-Segmentanzeige

     verwendet 8-Bit Timer2

     Anschluesse:
       PB0 .. PB6 : Segmente a -- g
       PB7        : Dezimalpunkt
       PA0        : MPX Digit 0 (LSB)
       PA7        : MPX Digit 1
       PA6        : MPX Digit 2 (MSB)

     Compiler  : SDCC 4.0.3
     MCU       : PFS154 / PFS173

     18.12.2020        R. Seelig

  -------------------------------------------------------- */

#include <stdint.h>
#include "pdk_init.h"
#include "pfs1xx_gpio.h"
#include "delay.h"
#include "seg7mpx_dig3.h"


/* --------------------------------------------------------
                       interrupt

     der Interrupt-Handler. TM2 ruft den Interrupt mit
     ca. 1 kHz auf.

     Hier muss das Multiplexen der Anzeige eingehaengt
     werden.
   -------------------------------------------------------- */
void interrupt(void) __interrupt(0)
{
  // Interruptquelle ist Timer2
  if (INTRQ & INTRQ_TM2)
  {
    seg7_mpx();                   // Timergesteuertes multiplexen
                                  // der 2-stelligen Anzeige
    INTRQ &= ~INTRQ_TM2;          // Interruptanforderung quittieren
  }
}

/* --------------------------------------------------------
                              main
   -------------------------------------------------------- */
void main(void)
{
  static uint16_t cx;

  delay(300);
  seg7_init();

  cx= 0;
  seg7_dp= 1;
  while(1)
  {
    s7buf= hex2bcd16(cx);
    delay(100);
    __disgint();
    cx++;
    cx= cx % 1000;
    __engint();
  }

}
