/*   -------------------------------------------------------
                          hd44780.h

     Header fuer grundlegende Funktionen eines HD44780
     kompatiblen LC-Textdisplays

     Hardware : Text-LCD

     MCU      :  PFS154
     Takt     :  8 MHz intern

     15.12.2020  R. Seelig

   ------------------------------------------------------- */

/* -------------------------------------------------------
      Pinbelegung
      Anschluss Pollin-Display C0802-04 an den Controller
    ---------------------------------------------------
       o +5V
       |                            Display                    PFS154
       _                        Funktion   PIN            PIN    Funktion
      | |
      |_|                          GND      1 ------------
       |                          +5V       2 ------------
       o----o Kontrast   ---    Kontrast    3 ------------
      _|_                           RS      4 ------------  13     PB0
      \ /                          GND      5 ------------
      -'-                    (Takt) E       6 ------------  14     PB1
       |                           D4       7 ------------  15     PB2
      --- GND                      D5       8 ------------  16     PB3
                                   D6       9 ------------  1      PB4
                                   D7      10 ------------  2      PB5

   ------------------------------------------------------- */


#ifndef in_txlcd
  #define in_txlcd

  #include <stdint.h>
  #include "pfs1xx_gpio.h"
  #include "delay.h"


  #define rs_init()      PB0_output_init()           // command / data
  #define e_init()       PB1_output_init()           // clock

  #define d4_init()      PB2_output_init()
  #define d5_init()      PB3_output_init()
  #define d6_init()      PB4_output_init()
  #define d7_init()      PB5_output_init()

  #define rs_set()       PB0_set()
  #define rs_clr()       PB0_clr()

  #define e_set()        PB1_set()
  #define e_clr()        PB1_clr()

  #define d4_set()       PB2_set()
  #define d4_clr()       PB2_clr()

  #define d5_set()       PB3_set()
  #define d5_clr()       PB3_clr()

  #define d6_set()       PB4_set()
  #define d6_clr()       PB4_clr()

  #define d7_set()       PB5_set()
  #define d7_clr()       PB5_clr()


  /* -------------------------------------------------------
       diverse Macros
     ------------------------------------------------------- */

  #define testbit(reg,pos) ((reg) & (1<<pos))               // testet an der Bitposition pos das Bit auf 1 oder 0


  /* -------------------------------------------------------
       Prototypen
     ------------------------------------------------------- */

    void txlcd_init(void);
    void txlcd_setuserchar(uint8_t nr, const uint8_t *userchar);
    void gotoxy(uint8_t x, uint8_t y);
    void txlcd_putchar(char ch);
    void txlcd_prints(uint8_t *c);

    extern uint8_t wherex,wherey;

#endif
