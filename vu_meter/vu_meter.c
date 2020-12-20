/*------------------------------------------------------------
                           leuchtband.c

     Einfache 8 stellige Leuchtbandanzeige unter Verwendung
     des Komparators und des internen programmierbaren
     Spannungsteilers.

     Das Leuchtband kann entweder logarithmisch oder
     linear unterteilt, bei logarithmischer Einteilung
     erfolgt ab -12 dBr eine schrittweise Erhoehung um
     3 dB

     PA4   : analoger Pegeleingang
     PB7:0 : 8 LED's aktiv low

     Beschreibung der Komparatorregister lt. Datenblatt
     in : comp_registers.txt

     Compiler  : SDCC 4.0.3
     MCU       : PFS154 / PFS173

     02.12.2020        R. Seelig
  ------------------------------------------------------------ */

#define  logscale         1             // "Skaleneinteilung"
                                        // 0 : linear
                                        // 1 : logarithmisch
#include <stdint.h>

#include "pdk_init.h"
#include "pfs1xx_gpio.h"
#include "delay.h"


// LED-Bandanzeige auf PortB, gemeinsame Anode an +Vcc

#define ledband_init()           ( PBC= 0xff )
#define ledband_set(value)       { PB= value; }

// Komparator Result-Bit

#define is_comp()                (GPCC & 0x40)

#if (logscale == 0)
//              lineare Einteilung
//              Ua_mux [V] :    0.15  0.31  0.47  0.62  0.78  0.94  1.09  1.25
  const uint8_t led_steps[] = { 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37 };
#endif

#if (logscale == 1)
//              logarithmische Einteilung
//              Ua_mux [V] :    0.20  0.31  0.47  0.62  1.04  1.40  2.00  2.81
  const uint8_t led_steps[] = { 0x10, 0x31, 0x32, 0x33, 0x14, 0x00, 0x27, 0x09 };
#endif


/* --------------------------------------------------------
                         comp_init
      Komparatorkonfiguration: -ue an Spannungsteiler
                               +ue nach P4
             enable    v-divider   PA4= +ue
                1    000   011        1
    GPCC =  (1 << 7 ) | (3 << 1) | (1 < 0)     // => 0x87
   -------------------------------------------------------- */
void comp_init(void)
{
  PAC &= ~MASK4;            // PA4 als Eingang
  PAPH &= ~MASK4;           // kein Pull-Up Widerstand
  PADIER_TMP &= ~MASK4;     // PA4 digital input disable
  PADIER= PADIER_TMP;       // Register schreiben

  GPCC= 0x87;                         // enable, -ue = Spg.Teiler, +ue = PA4
}


/* --------------------------------------------------------
                        dbr_getband

     legt die im Array db_steps gegebenen Werte an den
     Komparator und testet in aufsteigender Reihenfolge,
     ob der anliegende Pegel diesen Wert ueberschreitet
     und generiert durch Schiebeoperation das Bitmuster
     der Leuchtbandanzeige

     Rueckgabe: 8 stelliges Leuchtbandmuster
   -------------------------------------------------------- */
uint8_t led_getband(void)
{
  uint8_t i,w;

  w= 0;
  for (i= 0; i<8; i++)
  {
    GPCS= led_steps[i];
    if (is_comp()) w= (w << 1) | 1;
  }
  return w;
}


/* --------------------------------------------------------
                              main
   -------------------------------------------------------- */
void main(void)
{
  uint8_t w;

  comp_init();
  ledband_init();

  while(1)
  {
    w= led_getband();
    ledband_set(~w);
    delay(2);
  }
}
