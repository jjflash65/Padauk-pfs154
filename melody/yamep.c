/*--------------------------------------------------------
                          yamep.c

     Yet another melody player

     Spielt einen String, der Noten in C-Dur und der
     Tonlaenge enthaelt mittels Rechteckfrequenzen
     ab.

     Bsp.

            playstring("c4e2F4 + c2");

     spielt hintereinander eine Viertelnote "c", halbe
     Note "e" und eine Viertelnote "Fis" ab. Das Plus-
     zeichen "+" erhoeht die Oktave, so dass zum Schluss
     eine Halbnote "c" der hoeheren Oktave gespielt
     wird.

     Grosse Buchstaben C,D,F,G,A sind Noten entsprechend
     Cis, Dis, Fis, Gis und Ais.

     Der Anschluss eines Lautsprechers oder eines
     Lautsprechertreibers (Hardware) ist in toene.h
     anzugeben.

     Compiler  : SDCC 4.0.3
     MCU       : PFS154 / PFS173

     13.10.2020        R. Seelig

  -------------------------------------------------------- */

#include <stdint.h>
#include "pdk_init.h"
#include "pfs1xx_gpio.h"
#include "delay.h"
#include "toene.h"

const uint8_t schneew[] =       { "c4 d4 e2 g4 e2 g4 e1 d4"                 \
                                  "e4 f2 g4 f2 g4 f1 g4 a4"                 \
                                  "h2+f4-h2+f4-h1 a4 h4+c2"                 \
                                  "e4 c2-a4 g1 e4 g4+c1-h1"                 \
                                  "+d1 c1-a2+c4-a2+c4-a1" };



const uint8_t leise_rieselt[] = { "a4 a8 g8 a8 g8 f2 p8"                    \
                                  "f4 d8 f8 e8 d8 c2 p8"                    \
                                  "g8 F8 g8 A8 a8 g8 f2 p8"                 \
                                  "g8 d9 d8 e8 d8 e8 f2" };

/* --------------------------------------------------
                           main
   -------------------------------------------------- */
void main(void)
{
  char led_stat = 0;

  fout_init();
  t16_init();

  playtempo= 5;

  playstring(&schneew[0]);

  delay(1000);

  playtempo= 12;
  playstring(&leise_rieselt[0]);

  while(1);
}
