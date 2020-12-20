/*--------------------------------------------------------------
                          seg7mpx_dig4.c

     Softwaremodul einer 4-stelligen gemultiplexten
     7-Segmentanzeige mit gemeinsamer Kathode

     verwendet 8-Bit Timer2

     Compiler  : SDCC 4.0.3
     MCU       : PFS154 / PFS173

     Mittels defines (Kommentare beachten) kann zwischen
     Anzeigen mit gemeinsamer Anode und gemeinsamer Kathode
     umgeschaltet werden.

     Zusaetzlich zur Anzeige von Ziffern (0..9, A..F) ist es
     moeglich einen "Single-Segmentmodus" mit zu integrieren,
     mittels dessen es moeglich ist, jedes einzelne der 14
     Segmente getrennt anzusteuern.

     Sind beide Modi verfuegbar, schaltet ein

         segmode= zmode;

     den Ziffernmodus ein und der Inhalt der Variable s7buf
     wird hexadezimal auf der 2-stelligen Anzeige dargestellt.

         segmode= smode;

     schaltet den Segmentmodus ein und der Inhalt der Variable
     segbuf (16-Bit) beinhaltet das Bitmuster, das auf der Anzeige
     dargestellt wird:

          segbuf= 0x0107;

     laesst Segment a des hoeherwertigen Digits, die Segmente
     a,b,c des niederwertigen Digits anzeigen

     26.10.2020        R. Seelig
  -------------------------------------------------------------- */

#include "seg7mpx_dig4.h"


// --------------------------------------------------------
// globale Variable
// --------------------------------------------------------

  // Bitmuster fuer 7-Segmentanzeige mit gemeinsamer Kathode
const uint8_t s7bmp[] =               \
  {  0x3f,  0x06,  0x5b,  0x4f,       \
     0x66,  0x6d,  0x7d,  0x07,       \
     0x7f,  0x6f,  0x77,  0x7c,       \
     0x39,  0x5e,  0x79,  0x71 };

uint16_t s7buf= 0;                               // nimmt den Zahlenwert auf, der hexadezimal auf der
                                                 // 2-stelligen Anzeige angezeigt wird
extern uint8_t seg7_dp= 0;                       // bei verfuegbarem Dezimalpunkt nimmt diese


/* --------------------------------------------------------
                            seg7_init

     initialisiert den Timer2 (8-Bit) fuer einen Interrupt-
     intervall mit 1 kHz und die benoetigten Portanschluesse
     fuer die Anzeige als Ausgaenge
   -------------------------------------------------------- */
void seg7_init(void)
{
  seg7_pin_init();                              // Anschluesse fuer 7-Segmentanzeige als Ausgang

  TM2C = (uint8_t)TM2C_CLK_IHRC;
  TM2S = (uint8_t)TM2S_PRESCALE_DIV16 | TM2S_SCALE_DIV4;
  TM2B = 0x80;

  __engint();                   // grundsaetzlich Interrupt zulassen
  INTEN |= INTEN_TM2;           // Timerinterrupt zulassen
}

/* --------------------------------------------------------
                         seg7_mpx

     multiplext die 2-stellige 7-Segmentanzeige.

     Im Ziffernmodus (segmode== zmode) wird der Wert in
     s7buf automatisch ausgegeben, im Segmentmodus
     (segmode== smode) wird das Bitmuster aus segbuf
     automatisch ausgegeben
   -------------------------------------------------------- */
void seg7_mpx(void)
{
  static uint8_t sw_flag = 0;

  mpx0_clr(); mpx1_clr(); mpx2_clr(); mpx3_clr();

  switch (sw_flag)
  {
    case 0 :
    {
      PB = s7bmp[s7buf & 0x0f];
      mpx0_set();
      break;
    }

    case 1 :
    {
      PB = s7bmp[(s7buf >> 4) & 0x0f];
      if (seg7_dp == 1) dp_set();
      mpx1_set();
      break;
    }

    case 2 :
    {
      PB = s7bmp[(s7buf >> 8) & 0x0f];
      if (seg7_dp == 2) dp_set();
      mpx2_set();
      break;
    }

    case 3 :
    {
      PB = s7bmp[(s7buf >> 12) & 0x0f];
      if (seg7_dp == 3) dp_set();
      mpx3_set();
      break;
    }
    default :
    {
      PB = 0;
      break;
    }
  }

  sw_flag++;
  sw_flag= sw_flag % 6;
}

/* --------------------------------------------------------
                          hex2bcd16

     konvertiert einen hexadezimalen Wert in einen vier-
     stelligen dezimalen Wert, der auf der Anzeige ange-
     zeigt werden kann.

     Uebergabe:
        value   : hexadezimaler Zahlenwert
     Rueckgabe:
        dezimal korrigierter Wert

     Bsp.:
       v= hex2bcd(3419);

       v hat den Wert 0x3419 !! (wuerde dann dezimal
                                 13337 entsprechen)


       Aus 34 dezimal wurde 34 hexadezimal
   -------------------------------------------------------- */
uint16_t hex2bcd16(uint16_t value)
{
  uint16_t tmpz, tmpe;

  INTEN &= ~INTEN_TM2;           // Timerinterrupt sperren
  tmpe  = value;
  tmpz  = tmpe % 10;
  tmpe  /= 10;
  tmpz |= (tmpe % 10) << 4;
  tmpe  /= 10;
  tmpz |= (tmpe % 10) << 8;
  tmpe  /= 10;
  tmpz |= (tmpe % 10) << 12;
  INTEN |= INTEN_TM2;           // Timerinterrupt zulassen
  return tmpz;
}
