/* -------------------------------------------------------
                         charlie20.h

     Header fuer Softwaremodul zur Ansteuerung von
     20 LED's mittels Charlieplexing

     Compiler  : SDCC 4.0.3
     MCU       : PFS154 / PFS173

     19.10.2020        R. Seelig
   ------------------------------------------------------ */

/* -------------------------------------------------------------------------------
                        Charlieplexing 20-LED Modul

     Beim Charlieplexing sind jeweils 2 antiparallel geschaltete Leuchtdioden
     an 2 GPIO-Leitungen angeschlossen. Hieraus ergeben sich 10 Paare zu jeweils
     2 LEDs (bei 5 verwendeten GPIO-Anschluessen).

     Bsp.:

      A  o------+-----,         C o------+-----,
                |     |                  |     |
               ---   ---                ---   ---
             A \ /   / \ B            C \ /   / \ D
             B ---   --- A            D ---   --- C
                |     |                  |     |
      B  o------+-----'         D o------+-----'


      A  B  B  C  C  D  D  E  A  C  C  E  D  B  A  D  A  E  E  B   Linenkombination
      B  A  C  B  D  C  E  D  C  A  E  C  B  D  D  A  E  A  B  E
      ----------------------------------------------------------
      O  O  O  O  O  O  O  O  O  O  O  O  O  O  O  O  O  O  O  O   LED
      ----------------------------------------------------------
      0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19  LED-Nummern
   ------------------------------------------------------------------------------- */

#ifndef in_charlie20
  #define in_charlie20

  #include <stdint.h>
  #include <pdk/device.h>
  #include "pfs1xx_gpio3.h"

/* -------------------------------------------------------
     wichtigte globale Variable:
     Ein Beschreiben dieser Variablen fuehrt bei durch-
     gefuehrte Initialisierung zum Anzeigen des Bitmusters
     des Inhaltes von charlie20_buf
   ------------------------------------------------------- */
  extern uint32_t charlie20_buf;                  // Buffer in dem ein Bitmuster aufgenommen wird,
                                                  // welches ueber den Timerinterrupt ausgegeben wird

/* -------------------------------------------------------
                        Prototypen
   ------------------------------------------------------- */
  void charlie20_allinput(void);
  void charlie20_init(void);
  void charlie20_lineset(char nr);
  void charlie20_mpxlines(void);

  void t16_init(void);

  // Zuordnung LEDs zu GPIO-Anschluessen, Makros zum Setzen / Ruecksetzen von einzelnen
  // GPIO-Leitungen

  #define charlieA_output()  PB3_output_init()
  #define charlieA_input()   PB3_float_init()
  #define charlieA_set()     PB3_set()
  #define charlieA_clr()     PB3_clr()

  #define charlieB_output()  PB4_output_init()
  #define charlieB_input()   PB4_float_init()
  #define charlieB_set()     PB4_set()
  #define charlieB_clr()     PB4_clr()

  #define charlieC_output()  PB5_output_init()
  #define charlieC_input()   PB5_float_init()
  #define charlieC_set()     PB5_set()
  #define charlieC_clr()     PB5_clr()

  #define charlieD_output()  PB6_output_init()
  #define charlieD_input()   PB6_float_init()
  #define charlieD_set()     PB6_set()
  #define charlieD_clr()     PB6_clr()

  #define charlieE_output()  PB7_output_init()
  #define charlieE_input()   PB7_float_init()
  #define charlieE_set()     PB7_set()
  #define charlieE_clr()     PB7_clr()

  #define charlie20_bufset(nr)   (charlie20_buf |=  ( (uint32_t)1 << (uint32_t)nr ))
  #define charlie20_bufclr(nr)   (charlie20_buf &= ~( (uint32_t)1 << (uint32_t)nr ))

#endif
