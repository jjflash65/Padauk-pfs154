/*--------------------------------------------------------------
                          seg7mpx_dig4.h

     Header fuer Softwaremodul einer 4-stelligen gemultiplexten
     7-Segmentanzeige mit gemeinsamer Kathode

     verwendet 8-Bit Timer2

     Compiler  : SDCC 4.0.3
     MCU       : PFS154 / PFS173

     Mittels defines (Kommentare beachten) kann zwischen
     Anzeigen mit gemeinsamer Anode und gemeinsamer Kathode
     umgeschaltet werden.

     02.11.2020        R. Seelig
  -------------------------------------------------------------- */

#ifndef in_seg7mpx_dig4
  #define in_seg7mpx_dig4

  #include <stdint.h>
  #include <pdk/device.h>
  #include "pfs1xx_gpio.h"

  // --------------------------------------------------------
  //
  //   Anschlussbelegung der 7-Segmentanzeige an den
  //                    Controller
  //
  // PB0..PB6 fuer die 7-Segmentanzeige;
  // PB0: Segment a, B6: Segment g
  // Pxx: Dezimalpunk (DP)
  // PA0: MPX Digit0
  // PA7: MPX Digit1
  // PA6: MPX Digit2
  // PA5: MPX digit3
  // --------------------------------------------------------

  #define seg7_pin_init()     { PBC= 0xff; PAC= 0xf1; }

  #define mpx0_set()        PA0_clr()
  #define mpx0_clr()        PA0_set()
  #define mpx1_set()        PA7_clr()
  #define mpx1_clr()        PA7_set()
  #define mpx2_set()        PA6_clr()
  #define mpx2_clr()        PA6_set()
  #define mpx3_set()        PA5_clr()
  #define mpx3_clr()        PA5_set()

  #define dp_set()          ( PB7_set() )
  #define dp_clr()          ( PB7_clr() )

  enum {zmode, smode};

  // --------------------------------------------------------
  // globale Variable
  // --------------------------------------------------------
  extern uint16_t s7buf;                          // nimmt den Zahlenwert auf, der hexadezimal auf der
                                                  // 2-stelligen Anzeige angezeigt wird
                                                  // Bit7 nimmt bei verfuegbarem Dezimalpunkt eben diesen auf

  extern uint8_t seg7_dp;                         // nimmt die Position des Dezimalpunktes auf

  // --------------------------------------------------------
  // Prototypen
  // --------------------------------------------------------

  void seg7_init(void);
  void seg7_mpx(void);
  uint16_t hex2bcd16(uint16_t value);

#endif
