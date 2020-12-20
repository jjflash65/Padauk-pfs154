/*--------------------------------------------------------
                          toene.c

     Softwaremodul Notentongenerator und Abspielfunktion
     fuer in einem String enthaltene Noten

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

#include "toene.h"

volatile uint16_t   reload;
volatile char       sound = 0;
uint16_t            playtempo = 5;


// Reloadwerte fuer die Frequenzen der Noten, beginnend mit Frequenz fuer c''
// (12 Noten per Oktave) 2 Oktaven gespeichert = 24 Werte
uint16_t freqreload [24] = { 0x090b, 0x0fdb, 0x15fb, 0x1c00, 0x21bb, 0x26e0,
                             0x2be0, 0x3080, 0x3510, 0x3980, 0x3d60, 0x4140,
                             0x44a0, 0x47d0, 0x4b00, 0x4e20, 0x50e0, 0x5370,
                             0x55f0, 0x5870, 0x5a90, 0x5cb5, 0x5eb0, 0x6092 };
                             // naechster Wert waere 0x6257 fuer 1046 Hz


/* --------------------------------------------------------
                      t16_setnewfreq

    schreibt den 16-Bit Wert aus der Reloadfrequenztabelle
    in die vom SDCC System in timer16.h verwendete Variable
    __t16c die innerhalb dieses Programms somit als
    Reload-Variable fuer den 16-Bit Timer fungiert.

    Diese Funktion ist notwendig, da ein Speichern auf
    __sfr16 in SDCC (noch) nicht implementiert ist
   -------------------------------------------------------- */
void t16_setnewfreq(uint8_t freq)
{
  reload= freqreload[freq];
  __asm
      mov a,_reload+0
      mov __t16c+0,a
      mov a,_reload+1
      mov __t16c+1,a
  __endasm;
}

/* --------------------------------------------------------
                            t16_init

     initialisiert den 16-Bit Timer mit Interruptaus-
     loesung.
   -------------------------------------------------------- */
void t16_init(void)
{
  // interne RC-Taktquelle, kein Taktteiler, Ueberlauf beim hoechsten (15) Bit
  T16M = (uint8_t)(T16M_CLK_IHRC | T16M_CLK_DIV1 | T16M_INTSRC_15BIT);

  __engint();                   // grundsaetzlich Interrupt zulassen
  INTEN |= INTEN_T16;           // Timerinterrupt zulassen
}

/* --------------------------------------------------------
                       interrupt

     der Interrupt-Handler

     Leider hat sich (scheinbar) ein BUG in SDCC 4.0.3
     bzgl. des Schreibens eines 16-Bit Timerwertes ein-
     geschlichen und deshalb wird das Zaehlerregister
     des Timers mittels Maschinenbefehle beschrieben

   -------------------------------------------------------- */
void interrupt(void) __interrupt(0)
{

  if (INTRQ & INTRQ_T16)
  {

    // Reloadwert des 16-Bit Timers setzen
    __asm
        stt16 __t16c
    __endasm;

    if (sound)
      fout_toggle();
    else
      fout_clr();                 // wird Lautsprecher mittels PNP angeschlossen hier fout_set() angeben

    INTRQ &= ~INTRQ_T16;          // Interruptanforderung quittieren
  }
}

/* --------------------------------------------------
                          tonlen
      schaltet nach der Zeit - w - die Frequenz aus
      (und bestimmt somit, wie lange eine Note ge-
      spielt wird). Anschliessend erfolgt eine Warte-
      zeit.
   -------------------------------------------------- */
void tonlen(int w)
{
  volatile uint32_t cx;
  volatile uint32_t w2;

  w= 100 * w;
  for (cx= 0; cx< w; cx++)
    for (w2= 0; w2< 23; w2++);
  sound= 0;
  _delay_ms(30);
}


/* --------------------------------------------------
                       playnote
     erzeugt eine Frequenz entsprechend der Noten-
     tabelle. Frequenz wird solange erzeugt, wie die
     globale Variable - sound - groesser Null ist.
   -------------------------------------------------- */
void playnote(char note)
{
  t16_setnewfreq(note);
  sound= 1;
}

/* --------------------------------------------------
                       playstring
     Interpretiert einen String als "Notenstring".
     Gueltig abspielbare Noten sind:

     c-d-e-f-g-a-h-c
         und
     C-D-F-G-A  (fuer Cis, Dis, Fis, Gis, Ais)

     Gueltige Werte fuer Notenlaengen sind:

     1-2-3-4-5-8

     Bspw. wird mit "d4" eine Viertelnote D gespielt

     + schaltet in die hoehere Oktave
     - schaltet in die untere Oktave
   -------------------------------------------------- */
void playstring(const uint8_t* const s)
{
  char ch;
  char aokt;
  int dind;

  aokt= 0; dind= 0;
  ch= s[dind];
  while (ch)
  {
    ch= s[dind];
    switch(ch)
    {
      case '-': { aokt= aokt-12; break; }
      case '+': { aokt= aokt+12; break; }
      case 'c': { playnote(aokt); break; }
      case 'C': { playnote(aokt+1); break; }
      case 'd': { playnote(aokt+2); break; }
      case 'D': { playnote(aokt+3); break; }
      case 'e': { playnote(aokt+4); break; }
      case 'f': { playnote(aokt+5); break; }
      case 'F': { playnote(aokt+6); break; }
      case 'g': { playnote(aokt+7); break; }
      case 'G': { playnote(aokt+8); break; }
      case 'a': { playnote(aokt+9); break; }
      case 'A': { playnote(aokt+10); break; }
      case 'h': { playnote(aokt+11); break; }
      case '1': { tonlen(16*playtempo); break; }
      case '2': { tonlen(8*playtempo); break; }
      case '3': { tonlen(6*playtempo); break; }
      case '4': { tonlen(4*playtempo); break; }
      case '5': { tonlen(3*playtempo); break; }
      case '8': { tonlen(2*playtempo); break; }
    }
    dind++;
  }
}
