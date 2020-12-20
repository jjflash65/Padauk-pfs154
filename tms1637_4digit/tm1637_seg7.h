/* ------------------------------------------------------------------
                            tm1637_seg7.h

     Header zum Ansprechen des TM1637, Treiberbaustein der 4
     stelligen  7-Segmentanzeige (China) mit I2C aehnlichem
     Interface

     MCU   : PFS154


     18.12.2018 R. Seelig
   ------------------------------------------------------------------ */

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

#ifndef in_tm1637
  #define in_tm1637

  #include <stdint.h>
  #include "pfs1xx_gpio.h"
  #include "delay.h"


  /* ----------------------------------------------------------
            Anschluss des Moduls an den Controller
     ---------------------------------------------------------- */

  #define scl_init()     PA7_output_init()
  #define sda_init()     PA6_output_init()

  #define bb_sda_hi()    PA6_set()
  #define bb_sda_lo()    PA6_clr()

  #define bb_scl_hi()    PA7_set()
  #define bb_scl_lo()    PA7_clr()

  #define puls_us        5
  #define puls_len()     delay_us(puls_us)

  /* ----------------------------------------------------------
                       Globale Variable
     ---------------------------------------------------------- */

  extern uint8_t hellig;                  // beinhaltet Wert fuer die Helligkeit (erlaubt: 0x00 .. 0x0f);
  extern uint8_t tm1637_dp;               // 0 : Doppelpunkt abgeschaltet
                                          // 1 : Doppelpunkt sichtbar
                                          //     tm1637_dp wird beim Setzen der Anzeigeposition 1 verwendet
                                          //     und hat erst mit setzen dieser Anzeige einen Effekt

  extern uint8_t  led7sbmp[16];           // Bitmapmuster fuer Ziffern von 0 .. F


  /* ----------------------------------------------------------
                           PROTOTYPEN
     ---------------------------------------------------------- */

  void tm1637_start(void);
  void tm1637_stop(void);
  void tm1637_write (uint8_t value);
  void tm1637_init(void);
  void tm1637_clear(void);
  void tm1637_selectpos(char nr);
  void tm1637_setbright(uint8_t value);
  void tm1637_setbmp(uint8_t pos, uint8_t value);
  void tm1637_setzif(uint8_t pos, uint8_t zif);
  void tm1637_setseg(uint8_t pos, uint8_t seg);
  void tm1637_setdez(int value);
  void tm1637_sethex(uint16_t value);

#endif
