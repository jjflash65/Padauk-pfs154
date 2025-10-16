/* ---------------------------------------------------------
                         stxlcd.h

     Header Softwaremodul zum Ansprechen eines ueber einen
     Padauk PFS154 Controller gesteuerten Textdisplays
     mittels einer single-wire Verbindung.

     Datenleitung: PA4 (aenderbar)

     MCU      : PFS154
     Compiler : SDCC 4.01 oder neuer

     zusaetzliche Hardware:
        PFS154 - Textdisplay

     19.10.2024   R. Seelig
   --------------------------------------------------------- */

#ifndef in_txd_pfslcd
  #define in_txd_pfslcd

  #include <stdint.h>

  #include <stdint.h>
  #include "pfs1xx_gpio.h"
  #include "delay.h"

  extern uint8_t wherex, wherey;

  #define enable_stxt_puts      1             // 0 : nicht verfuegbar
                                              // 1 : verfuegbar

  #define enable_stxt_itoa      1             // 0 : nicht verfuegbar
                                              // 1 : verfuegbar

  /* --------------------------------------------------
       Pinanschluss, auf dem der PFS-Textdisplay-
       adapter angesteuert wird
     -------------------------------------------------- */

  #define stx_init()   { PA4_output_init(); stx_set(); }
  #define stx_set()    PA4_set()
  #define stx_clr()    PA4_clr()

  /* --------------------------------------------------
     Pulslaenge und -dauer in Mikrosekunden deklarieren
     -------------------------------------------------- */

  #define pulselength    70
  #define pulsepause     20

  /* --------------------------------------------------
       Prototypen
     -------------------------------------------------- */

  void stxt_putchar(uint8_t ch);
  void stxt_sendval(uint16_t val);
  void stxt_senduserchar(uint8_t nr, const uint8_t *userch);
  void gotoxy(uint8_t x, uint8_t y);
  void stxt_clrscr(void);

  void stxt_puts(char *p);
  void stxt_itoa(int i, char komma, char *puffer);


  /* --------------------------------------------------
       Kommandomakros
     -------------------------------------------------- */

  #define stxt_shiftleft(anz)       stxt_sendval(0x500 | anz)
  #define stxt_shiftright(anz)      stxt_sendval(0x600 | anz)
  #define stxt_addch(ch, pos)       { stxt_sendval(0x300 | (ch << 3) | pos); delay(4); }
  #define stxt_contrast(val)         stxt_sendval(0x700 | val)


#endif
