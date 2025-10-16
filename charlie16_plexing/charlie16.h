/* -------------------------------------------------------
                         charlie16.h

     Header fuer Softwaremodul zur Ansteuerung von
     16 LED's mittels Charlieplexing

     Hinweis: Es werden zur Ansteuerung von 16 LED's
     zwar 5 I/O Leitungen benoetigt und diese wuerden
     eine Ansteuerung von 20 LED's erlauben. Allerdings
     bedingt es fuer eine 20 stellige Anzeige einer
     32-Bit Variable und um diese zu Verrechnen steht
     in den allermeisten Faellen nicht genuegend
     RAM-Speicher zur Verfuegung (der PFS154 hat nur
     128 Byte Ram).

     Aus diesem Grund gibt es hier auch ein Charlie-
     plexing fuer 16 LED's


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
      0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 na na na na  LED-Nummern
   ------------------------------------------------------------------------------- */

#ifndef in_charlie16
  #define in_charlie16

  #include <stdint.h>
  #include <pdk/device.h>
  #include "pfs1xx_gpio3.h"

/* -------------------------------------------------------
     wichtigte globale Variable:
     Ein Beschreiben dieser Variablen fuehrt bei durch-
     gefuehrte Initialisierung zum Anzeigen des Bitmusters
     des Inhaltes von charlie16_buf
   ------------------------------------------------------- */
  extern uint32_t charlie16_buf;                  // Buffer in dem ein Bitmuster aufgenommen wird,
                                                  // welches ueber den Timerinterrupt ausgegeben wird

/* -------------------------------------------------------
                        Prototypen
   ------------------------------------------------------- */
  void charlie16_allinput(void);
  void charlie16_init(void);
  void charlie16_lineset(char nr);
  void charlie16_mpxlines(void);

  void t16_init(void);

  // Zuordnung LEDs zu GPIO-Anschluessen, Makros zum Setzen / Ruecksetzen von einzelnen
  // GPIO-Leitungen

  #define charlieA_output()  PB1_output_init()
  #define charlieA_input()   PB1_float_init()
  #define charlieA_set()     PB1_set()
  #define charlieA_clr()     PB1_clr()

  #define charlieB_output()  PB2_output_init()
  #define charlieB_input()   PB2_float_init()
  #define charlieB_set()     PB2_set()
  #define charlieB_clr()     PB2_clr()

  #define charlieC_output()  PB3_output_init()
  #define charlieC_input()   PB3_float_init()
  #define charlieC_set()     PB3_set()
  #define charlieC_clr()     PB3_clr()

  #define charlieD_output()  PB4_output_init()
  #define charlieD_input()   PB4_float_init()
  #define charlieD_set()     PB4_set()
  #define charlieD_clr()     PB4_clr()

  #define charlieE_output()  PB5_output_init()
  #define charlieE_input()   PB5_float_init()
  #define charlieE_set()     PB5_set()
  #define charlieE_clr()     PB5_clr()

  #define charlie16_bufset(nr)   (charlie16_buf |=  ( (uint16_t)1 << (uint16_t)nr ))
  #define charlie16_bufclr(nr)   (charlie16_buf &= ~( (uint16_t)1 << (uint16_t)nr ))

#endif
