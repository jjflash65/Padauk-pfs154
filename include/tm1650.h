/* ------------------------------------------------------------------
                            tm1650.h

     Header zum Ansprechen des TM1650  Treiberbaustein fuer
     7-Segmentanzeigen

     MCU   : PFS154

     CLK und DIO sind mit jeweils 2,2 kOhm Pop-Up Wider-
     staenden nach +5V zu versehen

     08.11.2024 R. Seelig
   ------------------------------------------------------------------ */

/*
    Segmentbelegung der Anzeige:

        a
       ---
    f | g | b            Segment |  a  |  b  |  c  |  d  |  e  |  f  |  g  | Doppelpunkt (nur fuer POS1) |
       ---               ---------------------------------------------------------------------------------
    e |   | c            Bit-Nr. |  0  |  1  |  2  |  3  |  4  |  5  |  6  |              7              |
       ---
        d


    Bit 7 der 7-Segmentanzeige ist der Dezimalpunkt
*/

#ifndef in_tm1650
  #define in_tm1650

  #include <stdint.h>
  #include "pfs1xx_gpio.h"
  #include "delay.h"


  /* ----------------------------------------------------------
            Anschluss des Moduls an den Controller
     ---------------------------------------------------------- */

  #define sda_init()     PA0_output_init()
  #define scl_init()     PA4_output_init()

  #define bb_sda_hi()    PA0_set()
  #define bb_sda_lo()    PA0_clr()

  #define bb_scl_hi()    PA4_set()
  #define bb_scl_lo()    PA4_clr()

  #define puls_us        5
  #define puls_len()     delay_us(puls_us)

  #define sethex_enable  1        // 1 : Hexausgabe verfuegbar
                                  // 0 : nicht verfuegbar

  #define setdez_enable  1        // 1 : Dezimalausgabe verfuegbar
                                  // 0 : nicht verfuegbar

  /* ----------------------------------------------------------
                       Globale Variable
     ---------------------------------------------------------- */

  extern uint8_t hellig;                  // beinhaltet Wert fuer die Helligkeit (erlaubt: 0x00 .. 0x0f);

  extern uint8_t  led7sbmp[17];           // Bitmapmuster fuer Ziffern von 0 .. F


  /* ----------------------------------------------------------
                           PROTOTYPEN
     ---------------------------------------------------------- */

  void tm1650_start(void);
  void tm1650_stop(void);
  void tm1650_write (uint8_t value);
  void tm1650_init(void);
  void tm1650_clear(void);
  void tm1650_selectpos(char nr);
  void tm1650_setbright(uint8_t value);
  void tm1650_setbmp(uint8_t pos, uint8_t value);
  void tm1650_setzif(uint8_t pos, uint8_t zif);
  void tm1650_setseg(uint8_t pos, uint8_t seg);
  void tm1650_setdez(int value, uint8_t dp);
  void tm1650_sethex(uint16_t value);

#endif
