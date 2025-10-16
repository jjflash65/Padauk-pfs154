/* -------------------------------------------------------
                         charlie6.c

     Softwaremodul zur Ansteuerung von 16 LED's mittels
     Charlieplexing.


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


#include "charlie6.h"

const uint8_t cplex[6] =
  // HiNibble (D7..D4): definiert, welche der Charlieplexingleitungen Ausgaenge sind
  // LoNibble (D3..D0): definiert, welche Bits gesetzt oder geloescht sind
  //
  //    D07 D06 D05 D04 | D03 D02 D01 D00
  //    ------------------------------------------------------------------
  //         C   B   A         C   B   A
  //

  // Bsp.: 0x032
  //   A und B sind Ausgaenge (HiNibble= 3), B= 1, A= 0 => LED BA== Nr.1 leuchtet

  // Bsp.: 0x054
  //   A und C sind Ausgaenge (HiNibble= 5), A= 0, C= 1, => LED CA== Nr.3 leuchtet

  {
  //  0=AB  1=BA  2=AC  3=CA  4=BC  5=CB
      0x31, 0x32, 0x51, 0x54, 0x62, 0x64
  };

volatile uint16_t reload = 489;

volatile uint8_t charlie6_buf= 0;      // Buffer in dem ein Bitmuster aufgenommen wird,
                                       // welches ueber den Timerinterrupt ausgegeben wird


/* ------------------------------------------------------
                   charlie6_allinput

     schaltet alle am Charlieplexing beteiligten I/O
     Leitungen als Eingang
   ------------------------------------------------------ */
void charlie6_allinput(void)
{
  charlieA_input();
  charlieB_input();
  charlieC_input();
}

/* ------------------------------------------------------
                      charlie6_init

     initialisiert die I/O Leitungen des Charlieplexings
     und setzt alle Leitungen auf 1
   ------------------------------------------------------ */
void charlie6_init(void)
{
  charlie6_allinput();
  t16_init();
}

/* ------------------------------------------------------
                    charlie6_lineset

     schaltet eine einzelne LED an.

     nr:   Nummer der einzuschaltenden LED (0..19)
           nr== 16 => alle LED's werden ausgeschaltet
   ------------------------------------------------------ */
void charlie6_lineset(char nr)
{
  uint8_t   bl, bh;

  charlie6_allinput();
  if (nr== 6) return;                               // 6 = alle LED aus

  bh= cplex[nr] >> 4;
  bl= cplex[nr] & 0x07;

  if (bh & 0x01) charlieA_output();
  if (bh & 0x02) charlieB_output();
  if (bh & 0x04) charlieC_output();

  if (bl & 0x01) charlieA_set(); else charlieA_clr();
  if (bl & 0x02) charlieB_set(); else charlieB_clr();
  if (bl & 0x04) charlieC_set(); else charlieC_clr();
}

/* --------------------------------------------------------
                    charlie6_mpxlines

     multiplext den charlie6_buf auf die einzelnen
     Leuchtdiode-Linien (sollte von einem Timerinterrupt
     aufgerufen werden)
   -------------------------------------------------------- */
void charlie6_mpxlines(void)
{
  static uint8_t isr_cnt = 0;

  charlie6_lineset(6);                          // alle LED's aus

  if (charlie6_buf & (1 << isr_cnt))            // Bit in charlie6_buf gesetzt ?
  {
    charlie6_lineset(isr_cnt);                  // dann diese LED einschalten
  }

  isr_cnt++;
  if (isr_cnt== 6) { isr_cnt= 0; }
}

/* --------------------------------------------------------
                            t16_init

     initialisiert den 16-Bit Timer mit Interruptaus-
     loesung.

     Interruptintervall hier: 1 ms

   -------------------------------------------------------- */
void t16_init(void)
{

  T16M = (uint8_t)(T16M_CLK_IHRC | T16M_CLK_DIV1 | T16M_INTSRC_14BIT);

  // Reloadwert des 16-Bit Timers setzen
  __asm

    mov a,_reload+0
    mov __t16c+0,a
    mov a,_reload+1
    mov __t16c+1,a

    stt16 __t16c
  __endasm;

  __engint();                   // grundsaetzlich Interrupt zulassen
  INTEN |= INTEN_T16;           // Timerinterrupt zulassen
}
