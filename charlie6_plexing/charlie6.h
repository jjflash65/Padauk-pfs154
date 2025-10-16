/* -------------------------------------------------------
                         charlie6.h

     Header fuer Softwaremodul zur Ansteuerung von
     16 LED's mittels Charlieplexing

     Hinweis: Es werden zur Ansteuerung von 6 LED's
     3 GPIO-Anschluesse benoetigt

     Compiler  : SDCC 4.0.3
     MCU       : PFS154 / PFS173

     15.10.2025        R. Seelig
   ------------------------------------------------------ */

/* -------------------------------------------------------------------------------
                        Charlieplexing 6-LED Modul

     Beim Charlieplexing sind jeweils 2 antiparallel geschaltete Leuchtdioden
     an 2 GPIO-Leitungen angeschlossen. Hieraus ergeben sich 3 Paare zu jeweils
     2 LEDs (bei 5 verwendeten GPIO-Anschluessen).

     Bsp.:

      A  o------+-----,         A o------+-----,         B o------+-----,
                |     |                  |     |                  |     |
               ---   ---                ---   ---                ---   --
             A \ /   / \ B            A \ /   / \ C            B \ /   / \ C
             B ---   --- A            C ---   --- A            C  ---   --- B
                |     |                  |     |                  |     |
      B  o------+-----'         C o------+-----'         C o------+-----,


      A  B  A  C  B  C  Linenkombination
      B  A  C  A  C  B
      ----------------------------------------------------------
      O  O  O  O  O  O  LED
      ----------------------------------------------------------
      0  1  2  3  4  5  LED-Nummern
   ------------------------------------------------------------------------------- */

#ifndef in_charlie6
  #define in_charlie6

  #include <stdint.h>
  #include <pdk/device.h>
  #include "pfs1xx_gpio.h"

/* -------------------------------------------------------
     wichtigte globale Variable:
     Ein Beschreiben dieser Variablen fuehrt bei durch-
     gefuehrte Initialisierung zum Anzeigen des Bitmusters
     des Inhaltes von charlie16_buf
   ------------------------------------------------------- */
  extern volatile uint8_t charlie6_buf;         // Buffer in dem ein Bitmuster aufgenommen wird,
                                                // welches ueber den Timerinterrupt ausgegeben wird

/* -------------------------------------------------------
                        Prototypen
   ------------------------------------------------------- */
  void charlie6_allinput(void);
  void charlie6_init(void);
  void charlie6_lineset(char nr);
  void charlie6_mpxlines(void);
  void charlie6_test(void);

  void t16_init(void);

  // Zuordnung LEDs zu GPIO-Anschluessen, Makros zum Setzen / Ruecksetzen von einzelnen
  // GPIO-Leitungen

  #define charlieA_output()  PB0_output_init()
  #define charlieA_input()   PB0_input_init()
  #define charlieA_set()     PB0_set()
  #define charlieA_clr()     PB0_clr()

  #define charlieB_output()  PB1_output_init()
  #define charlieB_input()   PB1_input_init()
  #define charlieB_set()     PB1_set()
  #define charlieB_clr()     PB1_clr()

  #define charlieC_output()  PB2_output_init()
  #define charlieC_input()   PB2_input_init()
  #define charlieC_set()     PB2_set()
  #define charlieC_clr()     PB2_clr()

  #define charlie6_bufset(nr)   (charlie6_buf |=  ( 1 << nr ))
  #define charlie6_bufclr(nr)   (charlie6_buf &= ~( 1 << nr ))

#endif
