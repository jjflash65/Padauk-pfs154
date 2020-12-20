/*--------------------------------------------------------
                          toene.c

     Header Notentongenerator und Abspielfunktion fuer
     in einem String enthaltene Noten

     Bsp.

            playstring("c4e2F4 + c2");

     spielt hintereinander eine Viertelnote "c", halbe
     Note "e" und eine Viertelnote "Fis" ab. Das Plus-
     zeichen "+" erhoeht die Oktave, so dass zum Schluss
     eine Halbnote "c" der hoeheren Oktave gespielt
     wird.

     Grosse Buchstaben C,D,F,G,A sind Noten entsprechend
     Cis, Dis, Fis, Gis und Ais.

     Benoetigt den 16-Bit Timer

     Compiler  : SDCC 4.0.3
     MCU       : PFS154 / PFS173

     13.10.2020        R. Seelig

  -------------------------------------------------------- */

#ifndef in_tone
  #define in_toene

  #include <stdint.h>
  #include "pfs1xx_gpio.h"
  #include "delay.h"


  // Frequenzausgang-Anschlusspin
  #define   fout_pin     PA0

  // Flag zum An- Ausschalten der Tonerzeugung
  extern volatile char sound;
  extern uint16_t playtempo;


  // ----------------------------------------------------------------
  //                           Prototypen
  // ----------------------------------------------------------------

  void t16_setnewfreq(uint8_t freq);
  void t16_init(void);
  void interrupt(void) __interrupt(0);
  void tonlen(int w);
  void playnote(char note);
  void playstring(const uint8_t* const s);


  // ----------------------------------------------------------------
  // Praeprozessormacros um 2 Stringtexte zur weiteren Verwendung
  // innerhalb des Praeprozessors zu verknuepfen
  //
  // Bsp.:
  //        #define ionr      3
  //        #define ioport    CONC(PA, ionr)
  //
  //        ioport wird nun als "PA3" behandelt
  #define CONC2EXP(a,b)     a ## b
  #define CONC(a,b)        CONC2EXP(a, b)
  // ----------------------------------------------------------------

  #define fout_init()     CONC(fout_pin,_output_init())
  #define fout_set()      CONC(fout_pin,_set())
  #define fout_clr()      CONC(fout_pin,_clr())
  #define fout_toggle()   CONC(fout_pin,_toggle())

#endif
