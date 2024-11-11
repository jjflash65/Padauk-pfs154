/*--------------------------------------------------------
                        laufgame1.c

                      Lauflichtspiel


     Compiler  : SDCC 4.1.0
     MCU       : PFS154

                 +----------------+
             PB4 | 1   |___|   16 | PB3
             PB5 | 2           15 | PB2
             PB6 | 3           14 | PB1
             PB7 | 4  PFS154   13 | PB0
                  ----------------
             VDD | 5 (1)   (8) 12 | GND
             PA7 | 6 (2)   (7) 11 | PA0
     ICPDA / PA6 | 7 (3)   (6) 10 | PA4
       Vpp / PA5 | 8 (4)   (5)  9 | PA3 / ICPCK
                 +----------------+


     08.11.2024        R. Seelig
  -------------------------------------------------------- */

#include <stdint.h>
#include "pdk_init.h"
#include "pfs1xx_gpio.h"
#include "delay.h"

#define leds            PB                    // kompletter PortB fuer LEDs

#define button_init()   PA7_input_init()      // PA7 Spieltaste
#define is_button()     (!is_PA7())

#define fout_init()     PA6_output_init()     // Lautsprecheranschluss
#define fout_set()      PA6_set();
#define fout_clr()      PA6_clr();
#define fout_toggle()   PA6_toggle();

#define tone_freq       0x0900

#define speed           35
#define hits2win        10


volatile uint16_t   reload;
volatile char       sound = 0;


/* --------------------------------------------------------
                      t16_setnewfreq

    setzt den 16-Bit Counter auf einen vordefinierten Wert.
    Assembler ist notwendig, da zum Zeitpunkt der Programm-
    erstellung SDCC dieses noch nicht implementiert hatte
    (SDCC v4.1.0).

    Dieses ist somit der Reloadwert des Timers

    Je kleiner der Wert in < freq > desto tiefer der Ton
   -------------------------------------------------------- */
void t16_setnewfreq(uint16_t freq)
{
  reload= freq;
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

     Leider hat sich (scheinbar) ein BUG in SDCC 4.1.0
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
    {
      fout_toggle();
    }
    else
    {
      fout_clr();                 // wird Lautsprecher mittels PNP angeschlossen hier fout_set() angeben
    }
    INTRQ &= ~INTRQ_T16;          // Interruptanforderung quittieren
  }
}

/* --------------------------------------------------------
                         is_hit

     stellt fest, ob Taster gedrueckt wurde und ob an der
     angegebenen Position ein Treffer ist

     Uebergabe:
       pos : Position des Lauflichts

     Rueckgabe:
         0 : nicht gedrueckt
         1 : gedrueckt und Treffer
         2 : gedrueckt und daneben
   -------------------------------------------------------- */
uint8_t is_hit(uint8_t pos)
{
  if (is_button())
  {
    delay(50);       // entprellen der Taste
    while(is_button());
    delay(50);
    if ((pos== 0x08) || (pos== 0x10)) { return 1; } else { return 2; }
  }
  return 0;
}

/* --------------------------------------------------------
                         gameplay

      Pollingverzoegerungsschleife in der die Taste abge-
      fragt wird.

      Uebergabe:
         dtime : Verzoegerungszeit (und somit Geschwindig-
                 keit des Spiels)
         pos   : aktuelle Position des Lauflichts
      Rueckgabe:
        0 : keine Taste gedrueckt
        1 : gedrueckt und Treffer
        2 : gedrueckt und daneben
   -------------------------------------------------------- */
uint8_t gameplay(uint16_t dtime, uint8_t pos)
{
  uint16_t i;
  uint8_t  t;

  for (i= 0; i< dtime; i++)
  {
    delay(1);
    t= is_hit(pos);
    if (t) return t;
  }
  return 0;
}


/* --------------------------------------------------
                           main
   -------------------------------------------------- */
void main(void)
{
  uint8_t  cx;
  uint8_t  stp, score;
  uint16_t w;

  PBC= 0xff;                   // alle Portpins Port-B als Ausgang
  button_init();
  fout_init();                 // Lautsprecher-Pin Init
  t16_init();                  // Timer Init

  sound= 1;
  for (w= 0x0900; w< 0x6000; w += 0x400)
  {
    t16_setnewfreq(w);
    delay(50);
  }
  sound= 0;

  t16_setnewfreq(tone_freq);   // Reloadwert Timer
  sound= 0;

  delay(100);

  score= 0;
  while(1)
  {
    cx= 1;

    // Lauflicht nach links
    while((cx < 128) && (score < hits2win))
    {
      leds= cx;
      t16_setnewfreq(tone_freq);
      if ((cx== 0x08) || (cx== 0x10)) { sound= 1; } else { sound= 0; }
      stp= gameplay(speed, cx);
      if (stp)
      {
        if (stp== 1) score++;
        if (stp== 2) score= 0;
        delay(200);
      }
      cx = cx << 1;
    }

    // Lauflicht nach rechts
    cx= 128;
    while((cx > 2)  && (score < hits2win))
    {
      leds= cx;
      t16_setnewfreq(tone_freq);
      if ((cx== 0x08) || (cx== 0x10)) { sound= 1; } else { sound= 0; }
      stp= gameplay(speed, cx);
      if (stp)
      {
        if (stp== 1) score++;
        if (stp== 2) score= 0;
        delay(200);
      }
      cx = cx >> 1;
    }

    if (score== hits2win)
    {

      sound= 1;
      for (w= 0x0900; w< 0x6000; w += 0x400)
      {
        t16_setnewfreq(w);
        delay(50);
      }
      sound= 0;

      while(is_button());   // warten bis Taster losgelassen wurde
      delay(50);            // entprellen

      // blinken bis Taste fuer neues Spiel betaetigt wurde
      do
      {
        leds= 0xaa;
        delay(200);
        leds= 0x55;
        delay(200);
      } while( !(is_button()));
      delay(50);            // entprellen

      score= 0;
    }
  }
}
