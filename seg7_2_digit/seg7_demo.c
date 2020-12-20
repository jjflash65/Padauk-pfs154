/*--------------------------------------------------------
                        seg7_demo.c

     Demo fuer eine 2-stellige gemultiplexte
     7-Segmentanzeige

     verwendet 8-Bit Timer2

     Anschluesse:
       PB0 .. PB6 : Segmente a -- g
       PB7        : niederwertiges Digit
       PA7        : hoeherwertiges Digit

     Compiler  : SDCC 4.0.3
     MCU       : PFS154 / PFS173

     26.10.2020        R. Seelig

  -------------------------------------------------------- */

#include <stdint.h>
#include "pdk_init.h"
#include "pfs1xx_gpio.h"
#include "delay.h"
#include "seg7mpx_dig2.h"

/* --------------------------------------------------------
         Lauflichtmuster fuer die Segmentanzeige
   -------------------------------------------------------- */
#if (gKathode == 1)
  const uint16_t seglaufl[] = {0x0100, 0x0001, 0x0002, 0x0004, 0x0008, 0x0800, 0x1000, 0x2000};
#endif

#if (gAnode == 1)
  const uint16_t seglaufl[] = {~0x0100, ~0x0001, ~0x0002, ~0x0004, ~0x0008, ~0x0800, ~0x1000, ~0x2000};
#endif


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
  uint8_t i;

  seg7_init();

  segmode= 1;

  while(1)
  {
    i= 20;

    segmode= zmode;
    do
    {
      s7buf= hex2bcd(i);
      if (i >10) seg7_dp= 0; else seg7_dp= 1;
      i--;
      delay(500);
    } while (i);

    seg7_dp= 0;

    // die Anzeige "00" blinken lassen
    for (i= 0; i< 2; i++)
    {
      segmode= zmode;
      s7buf= 0;
      delay(300);

      segmode= smode;
      // "Bitmuster" fuer alle Segmente aus
      #if (gKathode == 1)
        segbuf= 0x0000;
      #endif

      #if (gAnode == 1)
        segbuf= 0xffff;
      #endif
      delay(300);
    }

    segmode= smode;
    // 8 Leuchtmuster vorhanden = 3 Durchlaeufe
    for (i= 0; i<24; i++)
    {
      segbuf= seglaufl[i % 8];
      delay(125);
    }
  }
}
