/* -------------------------------------------------------
                           hx1838.h

     Header Softwaremodul fuer Infrarotempfaenger HX1838
     (38kHz), der durch eine Fernbedienung seine Signale
     erhaelt.

     Realisierung unter Verwendung Timer2

     Hardware : HX1838 IR-Empfaenger

     MCU      : PFS154
     Takt     : 8 MHz

     Pinbelegung :

       Dout HX1839 ---- PB0


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
    PA0 / PB0 |-----------------+   |
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


#ifndef in_hx1838
  #define in_hx1838

  #include <stdint.h>
  #include "pfs1xx_gpio.h"

  // Anschlusspin des IR-Receivers (nur PB0 und PA0 moeglich)
  #define irin_init()        PA0_input_init()
  #define is_irin()          is_PA0()
  #define irin_int           INTEN_PA0

  // Timer2 defines
  #define tim2_clr()         (TM2CT= 0)
  #define tim2_getvalue()    TM2CT

  extern volatile uint16_t  ir_code;                      // Code des letzten eingegangenen 16-Bit Wertes
  extern volatile uint8_t   ir_newflag;                   // zeigt an, ob ein neuer Wert eingegangen ist


  // -------------------------------------------------------
  //                        Prototypen
  // -------------------------------------------------------
  void ir_init(void);
  void ir_receive(void);


#endif
