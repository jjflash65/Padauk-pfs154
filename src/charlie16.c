/* -------------------------------------------------------
                         charlie16.c

     Softwaremodul zur Ansteuerung von 16 LED's mittels
     Charlieplexing.

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
                        Charlieplexing 16-LED Modul

     Beim Charlieplexing sind jeweils 2 antiparallel geschaltete Leuchtdioden
     an 2 GPIO-Leitungen angeschlossen. Hieraus ergeben sich 10 Paare zu jeweils
     2 LEDs.

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
      ----------------------------------------------------------
      0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 na na na na  LED-Nummern
   ------------------------------------------------------------------------------- */


#include "charlie20.h"

const uint16_t cplex[20] =
  // HiByte: definiert, welche der Charlieplexingleitungen Ausgaenge sind
  // LoByte: definiert, welche Bits gesetzt oder geloescht sind
  //
  //    D15 D14 D13 D12 D11 D10 D09 D08 || D07 D06 D05 D04 D03 D02 D01 D00
  //    ------------------------------------------------------------------
  //                  E   D   C   B   A                  E   D   C   B   A
  //
  // Bsp.: 0x0302

  //   A und B sind Ausgaenge, B= 0, A= 1 => LED AB leuchtet

  {
  //  0=AB    1=BA    2=BC    3=CB    4=CD    5=DC    6=DE    7=ED
    0x0301, 0x0302, 0x0602, 0x0604, 0x0c04, 0x0c08, 0x1808, 0x1810,
  //  8=AC    9=CA   10=CE    11=EC  12=DB   13=BD   14=AD   15=DA
    0x0501, 0x0504, 0x1404, 0x1410, 0x0a08, 0x0a02, 0x0901, 0x0908,
  // 16=AE   17=EA   18=EB    19=BE
    0x1101, 0x1110, 0x1210, 0x1202
  };

volatile uint16_t reload = 489;

uint16_t charlie16_buf= 0;             // Buffer in dem ein Bitmuster aufgenommen wird,
                                       // welches ueber den Timerinterrupt ausgegeben wird


/* ------------------------------------------------------
                   charlie16_allinput

     schaltet alle am Charlieplexing beteiligten I/O
     Leitungen als Eingang
   ------------------------------------------------------ */
void charlie16_allinput(void)
{
  charlieA_input();
  charlieB_input();
  charlieC_input();
  charlieD_input();
  charlieE_input();
}

/* ------------------------------------------------------
                      charlie16_init

     initialisiert die I/O Leitungen des Charlieplexings
     und setzt alle Leitungen auf 1
   ------------------------------------------------------ */
void charlie16_init(void)
{
  charlie16_allinput();
  t16_init();
}

/* ------------------------------------------------------
                    charlie16_lineset

     schaltet eine einzelne LED an.

     nr:   Nummer der einzuschaltenden LED (0..19)
           nr== 16 => alle LED's werden ausgeschaltet
   ------------------------------------------------------ */
void charlie16_lineset(char nr)
{
  uint8_t   bl, bh;

  charlie16_allinput();
  if (nr== 16) return;                               // 16 = alle LED aus

  bh= cplex[nr] >> 8;
  bl= cplex[nr] &0x00ff;

  if (bh & 0x01)
  {
    charlieA_output();
    if (bl & 0x01) charlieA_set(); else charlieA_clr();
  }
  if (bh & 0x02)
  {
    charlieB_output();
    if (bl & 0x02) charlieB_set(); else charlieB_clr();
  }
  if (bh & 0x04)
  {
    charlieC_output();
    if (bl & 0x04) charlieC_set(); else charlieC_clr();
  }
  if (bh & 0x08)
  {
    charlieD_output();
    if (bl & 0x08) charlieD_set(); else charlieD_clr();
  }
  if (bh & 0x10)
  {
    charlieE_output();
    if (bl & 0x10) charlieE_set(); else charlieE_clr();
  }
}

/* --------------------------------------------------------
                    charlie16_mpxlines

     multiplext den charlie16_buf auf die einzelnen
     Leuchtdiode-Linien (sollte von einem Timerinterrupt
     aufgerufen werden)
   -------------------------------------------------------- */
void charlie16_mpxlines(void)
{

  static uint8_t isr_cnt = 0;

  charlie16_lineset(16);                         // alle LED's aus
  if (charlie16_buf & (1ul << isr_cnt))          // Bit in charlie16_buf gesetzt ?
  {
    charlie16_lineset(isr_cnt);                  // dann diese LED einschalten
  }
  isr_cnt++;
  isr_cnt= isr_cnt % 16;

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
