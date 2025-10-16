/* -------------------------------------------------------
                       ir_receiver_demo.c

     Versuche zu IR-Fernbedienungsempfaenger HX1838
     (38 kHz), Realisierung unter Verwendung Timer2

     Hardware : HX1838 IR-Empfaenger
              : 7-Segmentanzeige mit TM1637 Controller

     MCU      : PFS154
     Takt     : 8 MHz

     Pinbelegung :

       Dout HX1839 ---- PB0
       CLK - TM1637 --- PA7
       DIO - TM1637 --- PA6


     17.12.2020  R. Seelig
   ------------------------------------------------------ */

/*
      +5 V                                    +5 V
        ^                                       ^
        |                                       |
        |                                       |
        |                                       |
        |                                       |
        |                                       |
        |                                       |
        |                    +-------------+    |
        |                    |   HX1838    |    |
        |                    |             |    |
   -----------+              | OUT GND +5V |    |
              |              +-------------+    |
              |                 |   |   |       |
              |                 |   |   |       |
     PFS154   |                 |   |   +-------+
              |                 |   |
              |                 |   |
          PB0 |-----------------+   |
              |                     |
              |                     |
              |                     |
              |                     |
              |                     |
   -----------+                    ---
        |
        |
        |
       ---
*/

#include <stdint.h>
#include "pdk_init.h"
#include "pfs1xx_gpio.h"
#include "delay.h"
#include "tm1637_seg7.h"


#include "hx1838.h"


/* --------------------------------------------------------
                       interrupt

     der Interrupt-Handler. Innerhalb dieser Funktion
     muss bei mehreren Interruptquellen das Interrupt-
     request Register INTRQ ausgewertet werden.

     Hier verwendet als externer Interrupt 0 auf PA0
   -------------------------------------------------------- */
void interrupt(void) __interrupt(0)
{

  // externer Interupt 0 auf PA0
  if (INTRQ & INTRQ_PA0)
  {

    ir_receive();

    INTRQ &= ~INTRQ_PA0;
  }

}


/* ------------------------------------------------------------------------------
                                     M A I N
    ----------------------------------------------------------------------------- */
void main(void)
{
  ir_init();
  hellig= 11;
  tm1637_init();

  tm1637_sethex(0);

  while(1)
  {

    if (ir_newflag)
    {
      tm1637_sethex(ir_code);
      ir_newflag= 0;
    }
  }
}
