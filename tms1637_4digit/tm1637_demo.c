/* ----------------------------------------------------------
                         tm1637_demo.c

   Grundsaetzliches zur 4 stelligen 7-Segmentanzeige mit
   TM1637 Treiber - IC


   18.12.2018 R. Seelig
   ---------------------------------------------------------- */

/*
    4-stelliges 7-Segmentanzeigemodul mit TM1637

    Anzeigenposition 0 ist das linke Segment des Moduls

         +---------------------------+
         |  POS0  POS1   POS2  POS3  |
         |  +--+  +--+   +--+  +--+  | --o  GND
         |  |  |  |  | o |  |  |  |  | --o  +5V
         |  |  |  |  | o |  |  |  |  | --o  DIO
         |  +--+  +--+   +--+  +--+  | --o  CLK
         |      4-Digit Display      |
         +---------------------------+

    Segmentbelegung der Anzeige:

        a
       ---
    f | g | b            Segment |  a  |  b  |  c  |  d  |  e  |  f  |  g  | Doppelpunkt (nur fuer POS1) |
       ---               ---------------------------------------------------------------------------------
    e |   | c            Bit-Nr. |  0  |  1  |  2  |  3  |  4  |  5  |  6  |              7              |
       ---
        d

    Bit 7 der 7-Segmentanzeige POS1 ist der Doppelpunkt
*/

#include <stdint.h>
#include "pdk_init.h"
#include "pfs1xx_gpio.h"
#include "delay.h"
#include "tm1637_seg7.h"


/* ----------------------------------------------------------
                            MAIN
   ---------------------------------------------------------- */
void main(void)
{
  int16_t i, i2;

  hellig= 11;
  tm1637_init();

  while(1)
  {
    for (i= 10; i> -1; i--)               // Countdown-Zaehler
    {
      tm1637_dp= 1;                       // Doppelpunkt an
      tm1637_setdez(i);
      _delay_ms(500);
      tm1637_dp= 0;                       // Doppelpunkt aus
      tm1637_setdez(i);
      _delay_ms(500);
    }

    for (i= 0; i< 3; i++)                 // 0000 blinken lassen
    {
      tm1637_clear();
      _delay_ms(500);
      tm1637_setdez(0);
      _delay_ms(500);
    }

    for (i= 3; i> -1; i--)                // Ziffer wandern lassen
    {
      tm1637_clear();
      tm1637_setzif(i,0);
      _delay_ms(300);
    }

    tm1637_sethex(0x1b3a);                // Hex-Wert anzeigen
    _delay_ms(2000);

    tm1637_clear();
    for (i2= 0; i2< 8; i2++)              // Segmentlauflicht, 8 Durchlaeufe
    {
      for (i= 0; i< 6; i++)
      {
        tm1637_setseg(0,i);
        tm1637_setseg(3,i);
        _delay_ms(100);
      }
    }
  }
}
