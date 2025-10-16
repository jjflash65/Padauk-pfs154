/* ----------------------------------------------------------
                        dig2_7seg_sr.h

     Header fuer Softwaremodul zur Ansteuerung des 7-Segment
     Moduls mit Schieberegister SN74HC595 (2-Digit
     Steckbrettmodul)


     MCU       :  PFS154 / PFS173

     26.09.2024  R. Seelig
   ---------------------------------------------------------- */
/*
               PFS154 / PFS173
                Anschlusspins

              +----------------+
          PB4 | 1   |___|   16 | PB3
          PB5 | 2           15 | PB2
          PB6 | 3           14 | PB1
          PB7 | 4  PFS154   13 | PB0
              -------------
          VDD | 5 (1)   (8) 12 | GND
          PA7 | 6 (2)   (7) 11 | PA0
  ICPDA / PA6 | 7 (3)   (6) 10 | PA4
    Vpp / PA5 | 8 (4)   (5)  9 | PA3 / ICPCK
              +----------------+                 7


                    74HC595
                 Anschlusspins IC

                 +------------+
                 |            |
            out1 | 1   H   16 | +Vcc
            out2 | 2   E   15 | out0
            out3 | 3   F   14 | dat
            out4 | 4       13 | /oe
            out5 | 5   4   12 | strobe
            out6 | 6   0   11 | clk
            out7 | 7   9   10 | /rst
             gnd | 8   4    9 | out7s
                 |            |
                 +------------+

           Anschlussbelegung

     PFS154        Pin - SN74HC595
    (default)
   ----------------------------------------
                   16 Vcc
                   10 (/maser reset) = Vcc
      PB3    ...   14 (seriell data)
      PB2    ...   12 (strobe)
      PB3    ...   11 (clock)
                    9 (seriell out; Q7S)
                    8 GND



                      Zuordnung Anzeigensegmente zu SR-Ausgaengen
         a
        ___           SR-Out  | 7 | 6 | 5 | 4 | 3 | 2 | 1 |  0 |
      f|_g_|b         ------------------------------------------
      e|___|c         Segment | g | c | d | e | a | b | f | dp |
         d            ------------------------------------------

*/

#ifndef in_dig2_sr
  #define in_dig2_sr

  #include <stdint.h>
  #include "pfs1xx_gpio.h"

  // Bitmuster fuer 7-Segmentanzeigen. Nur eine von beiden steht zur Wahl

  #define  gAnode         0               // Bitmuster fuer gemeinsame Anode
  #define  gKathode       1               // Bitmuster fuer gemeinsame Kathode

  // soll Timer2 nicht verwendet werden, wird die Anzeige nicht gemultiplext
  // und das Digit 0 dauerhaft aktiviert. So waere dann immer noch eine
  // einstellige Ziffernanzeige moeglich. Eine Ziffernausgabe kann dann mit:
  //    sr_setvalue(s7bmp[wert]);
  // erfolgen.

  #define  timer2_enable  1

  // ----------------------------------------------------------------
  //   Anbindung Schieberegister <==> Microcontroller
  // ----------------------------------------------------------------
  #define srdat_init()      PB3_output_init()
  #define srdat_set()       PB3_set()
  #define srdat_clr()       PB3_clr()

  #define srclk_init()      PB1_output_init()
  #define srclk_set()       PB1_set()
  #define srclk_clr()       PB1_clr()

  #define srstrobe_init()   PB2_output_init()
  #define srstrobe_set()    PB2_set()
  #define srstrobe_clr()    PB2_clr()

  // ----------------------------------------------------------------
  //   Anbindung Multiplexleitung der Anzeige
  // ----------------------------------------------------------------
  #define comd0_init()      PB4_output_init()
  #define comd0_set()       PB4_set()
  #define comd0_clr()       PB4_clr()

  #define comd1_init()      PB5_output_init()
  #define comd1_set()       PB5_set()
  #define comd1_clr()       PB5_clr()


  // initialisert beteiligten Pins als Ausgaenge
  #define sr_init()         { srdat_init(); srclk_init(); srstrobe_init(); \
                              comd0_init(); comd1_init(); }


/* --------------------------------------------------------
     globale Variable
   -------------------------------------------------------- */

  extern volatile uint8_t digout_val[2];     // 2-stelliges Bitmuster, das auf der Anzeige ausgegeben wird
  extern const uint8_t s7bmp[];              // Bitmuster der 7-Segmentanzeige

  void sr_setvalue(uint8_t value);
  void dig2_sethex(uint8_t value);
  void dig2_setdez(uint8_t value);
  void dig2_init(void);
  void dig2_mpx(void);


#endif
