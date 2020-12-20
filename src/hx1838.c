/* -------------------------------------------------------
                           hx1838.h

     Header Softwaremodul fuer Infrarotempfaenger HX1838
     (38kHz), der durch eine Fernbedienung seine Signale
     erhaelt.

     Realisierung unter Verwendung Timer2

     Hardware : HX1838 IR-Empfaenger

     MCU      : PFS154
     Takt     : 8 MHz

     Pinbelegung :

       Dout HX1839 ---- PB0


     17.12.2020  R. Seelig
   ------------------------------------------------------ */

/*
      +5 V                                    +5 V
        ^                                       ^
        |                                       |
        |                                       |
        |                                       |
        |                                       |
        |                                       |
        |                                       |
        |                    +-------------+    |
        |                    |   HX1838    |    |
        |                    |             |    |
   -----------+              | OUT GND +5V |    |
              |              +-------------+    |
              |                 |   |   |       |
              |                 |   |   |       |
     PFS154   |                 |   |   +-------+
              |                 |   |
              |                 |   |
    PA0 / PB0 |-----------------+   |
              |                     |
              |                     |
              |                     |
              |                     |
              |                     |
   -----------+                    ---
        |
        |
        |
       ---
*/

#include "hx1838.h"

volatile uint16_t  ir_code;                                                 // Code des letzten eingegangenen 16-Bit Wertes
volatile uint8_t   ir_newflag;                                              // zeigt an, ob ein neuer Wert eingegangen ist


/* --------------------------------------------------------
                          tim2_init

     Beteiligte Register: TM2C (timer2 control register)
                          TM2C (timer2 counter register)
                          TM2S (timer2 scalar reigster)

     erzeugt einen Interrupt alle 16 ms
   -------------------------------------------------------- */
void tim2_init(void)
{
  // 0010         00               00                         = 0x20
  // IHRC  output sel. disable     periode mode / no invers
  TM2C = 0x20;

  // 0       00         01110            = 0x0e
  // 0       11         11111            = 0x7f
  // PWM-8   Prescale   clock divisior

  TM2S = 0x7f;

  TM2B = 0x80;

  __engint();                   // grundsaetzlich Interrupt zulassen
  TM2CT= 0x00;                  // Counter auf 0 setzen
}


/* --------------------------------------------------
                       ir_init

     initialisiert:

       - den Anschlusspin fuer den HX1839  IR-
         Empfaenger als Eingang, kann nur PA0 oder
         PB0 sein, da nur diese beiden interruptfaehig
         sind.
       - Timer2, nicht Interruptfaehig (wird zur
         Messung der Puls- und Pausezeiten verwendet
       - schaltet Interrupt fuer den Eingangspin
         ein. Interruptausloeser ist ein Pinchange
         auf diesem Pin (Datenblatt beschreibt es
         mit ausgeloest durch positive und negative
         Flanke)
   -------------------------------------------------- */
void ir_init(void)
{
  irin_init();
  tim2_init();
  INTEN |= irin_int;
  __engint();                   // grundsaetzlich Interrupt zulassen

}

/* --------------------------------------------------
                      waittil_hi

     wartet, bis die Signalleitung zu logisch 1
     wird.

     Uebergabe:
        timeout  : Zeit x 100us nach der nach nicht
                   eingehen eines Hi-Pegels ein
                   Timeout ausgeloest wird
   -------------------------------------------------- */
uint8_t waittil_hi(uint16_t timeout)
{
  tim2_clr();
  while ( (!(is_irin() )) && (timeout > tim2_getvalue()) );
  if (timeout > tim2_getvalue()) return 0; else return 1;
}

/* --------------------------------------------------
                      waittil_lo

     wartet, bis die Signalleitung zu logisch 0
     wird.

     Uebergabe:
        timeout  : Zeit x 100us nach der nach nicht
                   eingehen eines Lo-Pegels ein
                   Timeout ausgeloest wird
   -------------------------------------------------- */
uint8_t waittil_lo(uint16_t timeout)
{
  tim2_clr();
  while ( (is_irin() ) && (timeout > tim2_getvalue()) );
  if (timeout > tim2_getvalue()) return 0; else return 1;
}

/* --------------------------------------------------
                      ir_getbit

     liefert je nach Pulselaenge des IR-Receivers 0
     (kurzer Impuls) oder 1 (langer Impuls). Bei
     Timeoutueberschreitung wird 2 zurueck geliefert

     Uebergabe:
        timeout  : Zeit x 100us nach der nach nicht
                   eingehen eines Lo-Pegels ein
                   Timeout ausgeloest wird
     Rueckgabe   : 0,1  Datenbit
                     2  Timeout aufgetreten
   -------------------------------------------------- */
uint8_t ir_getbit(uint16_t timeout)
{
  volatile uint16_t t;

  tim2_clr();
  while ( (is_irin() ) && (timeout > tim2_getvalue()) );
  t= tim2_getvalue();
  if (timeout <= t) return 2;
  if (t< 9) return 0; else return 1;
}



/* --------------------------------------------------
                       ir_receive

    Zentrale Funktion, die in den Interrupt-Handler
    eingebunden werden muss und die Auswertung eines
    eingehenden Datums vornimmt.

    Wird ein gueltiges Datum empfangen, wird in die
    globale Variable ir_code dieses Datum gesetzt,
    die Variable ir_newflag wird mit 1 beschrieben
    um zu signalisieren, dass ein Datum eingegangen
    ist.
   -------------------------------------------------- */
void ir_receive(void)
{
  volatile uint8_t cx, b, hw;
  volatile uint16_t result;

  if (!(is_irin()) )                                   // ist der Datenpin des IR-Receivers zu 0 geworden
  {

    INTEN &= irin_int;                                 // Interruptfaehigkeit des Pins aus

    if ( waittil_hi(186) ) goto timeout_err;           // auf Startbit des Frames warten (nach 93*.128us = 12 ms Timeout)
    if ( waittil_lo(94) ) goto timeout_err;            // auf Ende Startbit des Frames warten (nach 6 ms Timeout)
    if ( waittil_hi(48) ) goto timeout_err;            // auf erstes Datenbit warten (nach 3 ms Timeout)

    // die ersten 16 Bit des Pakets einlesen ohne diese
    // auszuwerten
    for (cx= 0; cx < 16; cx++)
    {
      b= ir_getbit(48);
      if (b == 2) goto timeout_err;
      if ( waittil_hi(48) ) goto timeout_err;          // auf naechstes Datenbit warten
    }

    // erste 8 Bit seriell lesen
    hw= 0;
    for (cx= 0; cx < 8; cx++)
    {
      b= ir_getbit(48);
      if (b == 2) goto timeout_err;
      hw |= (b << cx);
      if ( waittil_hi(48) ) goto timeout_err;          // auf naechstes Datenbit warten
    }
    result= (uint16_t) hw << 8;

    // zweite 8 Bit seriell lesen
    hw= 0;
    for (cx= 0; cx < 8; cx++)
    {
      b= ir_getbit(48);
      if (b == 2) goto timeout_err;
      hw |= (b << cx);
      if ( waittil_hi(48) ) goto timeout_err;          // auf naechstes Datenbit warten
    }
    result |= hw;

    if (result < 0x8000)
    {
      ir_code= result;
      ir_newflag= 1;
    }

    timeout_err:

    INTEN |= irin_int;                                // Interrupt wieder an
  }
}
