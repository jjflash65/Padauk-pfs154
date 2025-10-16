/* ----------------------------------------------------------
                        dig2_7seg_sr.c

     Softwaremodul zur Ansteuerung des 7-Segment
     Moduls mit Schieberegister SN74HC595 (2-Digit
     Steckbrettmodul)

     Pinbelegung in dig2_7seg_sr.h aenderbar


     MCU       :  PFS154 / PFS173

     26.09.2024  R. Seelig
   ---------------------------------------------------------- */
/*
               PFS154 / PFS173
                Anschlusspins

              +----------------+
          PB4 | 1   |___|   16 | PB3
          PB5 | 2           15 | PB2
          PB6 | 3           14 | PB1
          PB7 | 4  PFS154   13 | PB0
              -------------
          VDD | 5 (1)   (8) 12 | GND
          PA7 | 6 (2)   (7) 11 | PA0
  ICPDA / PA6 | 7 (3)   (6) 10 | PA4
    Vpp / PA5 | 8 (4)   (5)  9 | PA3 / ICPCK
              +----------------+                 7


                    74HC595
                 Anschlusspins IC

                 +------------+
                 |            |
            out1 | 1   H   16 | +Vcc
            out2 | 2   E   15 | out0
            out3 | 3   F   14 | dat
            out4 | 4       13 | /oe
            out5 | 5   4   12 | strobe
            out6 | 6   0   11 | clk
            out7 | 7   9   10 | /rst
             gnd | 8   4    9 | out7s
                 |            |
                 +------------+

           Anschlussbelegung

     PFS154        Pin - SN74HC595
    (default)
   ----------------------------------------
                   16 Vcc
                   10 (/maser reset) = Vcc
      PB3    ...   14 (seriell data)
      PB2    ...   12 (strobe)
      PB3    ...   11 (clock)
                    9 (seriell out; Q7S)
                    8 GND



                      Zuordnung Anzeigensegmente zu SR-Ausgaengen
         a
        ___           SR-Out  | 7 | 6 | 5 | 4 | 3 | 2 | 1 |  0 |
      f|_g_|b         ------------------------------------------
      e|___|c         Segment | g | c | d | e | a | b | f | dp |
         d            ------------------------------------------

*/

#include <stdint.h>
#include "dig2_7seg_sr.h"
#include "pfs1xx_gpio.h"

volatile uint8_t digout_val[2] = {0,0};

#if (gKathode == 1)
  // Bitmuster fuer 7-Segmentanzeige mit gemeinsamer Kathode
  const uint8_t s7bmp[] =               \
    {  0x7e,  0x44,  0xbc,  0xec,       \
       0xc6,  0xea,  0xfa,  0x4c,       \
       0xfe,  0xee,  0xde,  0xf2,       \
       0x3a,  0xf4,  0xba,  0x9a,
       0x00 };
#endif

#if (gAnode == 1)
  // dto. fuer gemeinsame Anode
  const uint8_t s7bmp[] =
    {  ~0x7e,  ~0x44,  ~0xbc,  ~0xec,   \
       ~0xc6,  ~0xea,  ~0xfa,  ~0x4c,   \
       ~0xfe,  ~0xee,  ~0xde,  ~0xf2,   \
       ~0x3a,  ~0xf4,  ~0xba,  ~0x9a,
       0xff };
#endif

#if (timer2_enable == 1)
  /* --------------------------------------------------------
                           timer2_init

       initialisiert den Timer2 (8-Bit) fuer einen Interrupt-
       intervall mit ca. 60Hz
     -------------------------------------------------------- */
  void timer2_init(void)
  {
    TM2C = (uint8_t)TM2C_CLK_IHRC;
    TM2S = (uint8_t)TM2S_PRESCALE_DIV64 | TM2S_SCALE_DIV16;
    TM2B = 0x80;                  // compare match Wert

    __engint();                   // grundsaetzlich Interrupt zulassen
    INTEN |= INTEN_TM2;           // Timerinterrupt zulassen
  }
#endif

/* ----------------------------------------------------------
                             sr_setvalue

     transferiert einen 8-Bit Wert ins serielle Schiebe-
     register
   ---------------------------------------------------------- */
void sr_setvalue(uint8_t value)
{
  int8_t i;

  for (i= 7; i> -1; i--)
  {
    if ((value >> i) &1)  srdat_set(); else srdat_clr();

    srclk_set();
    srclk_clr();                            // Taktimpuls erzeugen
  }

  srstrobe_set();                           // Strobeimpuls : Daten Schieberegister ins Ausgangslatch uebernehmen
  srstrobe_clr();
}

/* ----------------------------------------------------------
                        dig2_mpx

     multiplext die Digits 0 und 1, wird vom Interrupt aus
     aufgerufen
   ---------------------------------------------------------- */
void dig2_mpx(void)
{
  volatile static uint8_t cx;

  #if (gKathode == 1)
   comd0_clr();
   comd1_clr();
   sr_setvalue(s7bmp[16]);
   cx++;
   if (cx & 0x01)
   {
     comd1_set();
     sr_setvalue(digout_val[1]);
   }
   else
   {
     comd0_set();
     sr_setvalue(digout_val[0]);
   }
 #else
   comd0_set();
   comd1_set();
   sr_setvalue(s7bmp[16]);
   cx++;
   if (cx & 0x01)
   {
     comd1_clr();
     sr_setvalue(digout_val[1]);
   }
   else
   {
     comd0_clr();
     sr_setvalue(digout_val[0]);
   }
 #endif
}


/* ----------------------------------------------------------
                        dig2_setdez

     gibt den Wert in <value> auf der 2-stelligen 7-Segment
     Anzeige dezimal aus
   ---------------------------------------------------------- */
void dig2_setdez(uint8_t value)
{
  uint8_t quo;

/*
  digout_val[0]= s7bmp[value % 10];
  digout_val[1]= s7bmp[value / 10];
*/

  quo= 0;

  // nachfolgende while-Schleife ersetzt eine Divisionsoperation / 10
  // und erstellt automatische das Ergebnis einer Modulo 10 Division.
  // (liefert dasselbe Ergebnis wie value % 10 und value / 10, spart
  // hierdurch 78 words Speicherplatz ein).
  while(value > 9)
  {
    value -= 10;
    quo++;
  }
  digout_val[0]= s7bmp[value];
  digout_val[1]= s7bmp[quo];

}


/* ----------------------------------------------------------
                        dig2_sethex

     gibt den Wert in <value> auf der 2-stelligen 7-Segment
     Anzeige hexadezimal aus
   ---------------------------------------------------------- */
void dig2_sethex(uint8_t value)
{
  digout_val[0]= s7bmp[value & 0x0f];
  digout_val[1]= s7bmp[(value & 0xf0) >> 4];
}

/* ----------------------------------------------------------
                        dig2_init

     initialisiert die Anschluesse des Schieberegisters und
     den Timer2 fuer Intervallfrequenz von ca. 60 Hz
   ---------------------------------------------------------- */
void dig2_init(void)
{
  sr_init();
  #if (timer2_enable == 1)
    timer2_init();
  #else
    #if (gKathode == 1)
      comd0_set();
      comd1_clr();
    #else
      comd1_set();
      comd0_clr();
    #endif
  #endif
}


