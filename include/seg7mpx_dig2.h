/*--------------------------------------------------------------
                          seg7mpx_dig2.h

     Header fuer Softwaremodul einer 2-stelligen gemultiplexten
     7-Segmentanzeige

     verwendet 8-Bit Timer2

     Compiler  : SDCC 4.0.3
     MCU       : PFS154

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

#ifndef in_seg7mpx_dig2
  #define in_seg7mpx_dig2

  #include <stdint.h>
  #include <pdk/device.h>
  #include "pfs1xx_gpio.h"


  // --------------------------------------------------------
  // Type der 7-Segmentanzeige (gemeinsame Anode oder Kathode)
  // --------------------------------------------------------

  #define gKathode          1                     // Anzeige fuer gemeinsame Kathode
  #define gAnode            0                     // dto. fuer gemeinsame Anode

  #define segmode_enable    1                     // 1 fuer einzelne Segmente ansteuerbar
                                                  // 0 nicht verfuegbar (spart Speicher)

  #define dp_enable         1                     // 1 Steuerung des Dezimalpunktes zwischen linker  // --------------------------------------------------------
                                                  //   und rechter Ziffer verfuegbar
                                                  // 0 nicht verfuegbar

  // --------------------------------------------------------
  //
  //   Anschlussbelegung der 7-Segmentanzeige an den
  //                    Controller
  //
  // PB0..PB6 fuer die 7-Segmentanzeige;
  // PB0: Segment a, B6: Segment g
  // PA0: Dezimalpunk (DP), wenn verfuegbar
  // PB7: Multiplexlinie fuer niederwertiges Digit
  // PA7: dto. fuer hoeherwertiges Digit
  // --------------------------------------------------------

  #if (dp_enable == 0)
    #define seg7_pin_init()     { PBC= 0xff; PA7_output_init(); }
  #else
    #define seg7_pin_init()     { PBC= 0xff; PA7_output_init(); PA0_output_init(); }
  #endif

  #if (gKathode == 1)
    #define seg7_setdig0()    { PA7_clr(); PB7_set(); }
    #define seg7_setdig1()    { PB7_clr(); PA7_set(); }
    #define dp_set()          ( PA0_set() )
    #define dp_clr()          ( PA0_clr() )
  #endif

  #if (gAnode == 1)
    #define seg7_setdig0()    { PB7_clr(); PA7_set(); }
    #define seg7_setdig1()    { PA7_clr(); PB7_set(); }
    #define dp_set()          ( PA0_clr() )
    #define dp_clr()          ( PA0_set() )
  #endif

  enum {zmode, smode};

  // --------------------------------------------------------
  // globale Variable
  // --------------------------------------------------------
  #if (segmode_enable == 1)

    extern uint16_t segbuf;                       // nimmt die Bits der einzelnen Segmente
                                                  // auf, niederwertiges Byte rechte Anzeige,
                                                  // hoeherwertiges Byte linke Anzeige
    extern uint8_t segmode;                       // 0 = Ziffernanzeige
                                                  // 1 = einzelne Segmente aus segbuf werden angezeigt
  #endif

  extern uint8_t s7buf;                           // nimmt den Zahlenwert auf, der hexadezimal auf der
                                                  // 2-stelligen Anzeige angezeigt wird
                                                  // Bit7 nimmt bei verfuegbarem Dezimalpunkt eben diesen auf

  #if (dp_enable == 1)
    extern uint8_t seg7_dp;                       // bei verfuegbarem Dezimalpunkt nimmt diese
                                                  // Variable den Status des Dezimalpunktes auf
  #endif

  // --------------------------------------------------------
  // Prototypen
  // --------------------------------------------------------

  void seg7_init(void);
  void seg7_mpx(void);
  uint8_t hex2bcd(uint8_t value);

#endif
