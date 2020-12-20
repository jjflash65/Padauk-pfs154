/*--------------------------------------------------------------
                          seg7mpx_dig2.c

     Softwaremodul einer 2-stelligen gemultiplexten
     7-Segmentanzeige

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

#include "seg7mpx_dig2.h"


// --------------------------------------------------------
// globale Variable
// --------------------------------------------------------

#if (gKathode == 1)
  // Bitmuster fuer 7-Segmentanzeige mit gemeinsamer Kathode
  const uint8_t s7bmp[] =               \
    {  0x3f,  0x06,  0x5b,  0x4f,       \
       0x66,  0x6d,  0x7d,  0x07,       \
       0x7f,  0x6f,  0x77,  0x7c,       \
       0x39,  0x5e,  0x79,  0x71 };
#endif

#if (gAnode == 1)
  // dto. fuer gemeinsame Anode
  const uint8_t s7bmp[] =               \
    { ~0x3f, ~0x06, ~0x5b, ~0x4f,       \
      ~0x66, ~0x6d, ~0x7d, ~0x07,       \
      ~0x7f, ~0x6f, ~0x77, ~0x7c,       \
      ~0x39, ~0x5e, ~0x79, ~0x71 };
#endif

#if (segmode_enable == 1)

  uint16_t segbuf = 0;                          // nimmt die Bits der einzelnen Segmente
                                                // auf, niederwertiges Byte rechte Anzeige,
                                                // hoeherwertiges Byte linke Anzeige
  uint8_t  segmode = 0;                         // 0 = Ziffernanzeige
                                                // 1 = einzelne Segmente aus segbuf werden angezeigt
#endif

uint8_t s7buf= 0;                               // nimmt den Zahlenwert auf, der hexadezimal auf der
                                                // 2-stelligen Anzeige angezeigt wird
#if (dp_enable == 1)
  extern uint8_t seg7_dp= 0;                    // bei verfuegbarem Dezimalpunkt nimmt diese
#endif


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

  PB &= 0x80;

  if (sw_flag== 0)               // linkes Segment
  {
    #if (segmode_enable == 1)
      // einzelne Segmente werden angezeigt
      if (segmode)
      {
        PB |= (segbuf >> 8);
      }
      // Ziffern werden angezeigt
      else
      {
        PB |= s7bmp[s7buf >> 4];
      }
    #else
      PB |= s7bmp[s7buf >> 4];
    #endif

    #if (dp_enable == 1)
      if (seg7_dp) dp_set(); else dp_clr();
    #endif

    seg7_setdig0();
    sw_flag= 1;
  }
  else                           // rechtes Segment
  {
    #if (segmode_enable == 1)
      // einzelne Segmente werden angezeigt
      if (segmode)
      {
        PB |= (segbuf & 0xff);
      }
      // Ziffern werden angezeigt
      else
      {
        PB |= s7bmp[s7buf & 0x0f];
      }
    #else
      PB |= s7bmp[s7buf & 0x0f];
    #endif

    #if (dp_enable == 1)
      dp_clr();
    #endif
    seg7_setdig1();
    sw_flag= 0;
  }
}

/* --------------------------------------------------------
                          hex2bcd

     konvertiert einen hexadezimalen Wert in einen zwei-
     stelligen dezimalen Wert, der auf der Anzeige ange-
     zeigt werden kann.

     Uebergabe:
        value   : hexadezimaler Zahlenwert
     Rueckgabe:
        dezimal korrigierter Wert

     Bsp.:
       v= hex2bcd(34);

       v hat den Wert 0x34 !! (wuerde dann dezimal 52 ent-
                               sprechen)

       Aus 34 dezimal wurde 34 hexadezimal
   -------------------------------------------------------- */
uint8_t hex2bcd(uint8_t value)
{
  uint8_t tmpz, tmpe;

  tmpe  = value;
  tmpz  = tmpe / 10;
  tmpe -= (tmpz*10);
  return ((tmpz << 4) | tmpe);
}
