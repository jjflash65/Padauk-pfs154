/*--------------------------------------------------------------
                          seg7mpx_dig3.h

     Header fuer Softwaremodul einer 4-stelligen gemultiplexten
     7-Segmentanzeige mit gemeinsamer Kathode

     verwendet 8-Bit Timer2

     Compiler  : SDCC 4.0.3
     MCU       : PFS154 / PFS173

     02.11.2020        R. Seelig
  -------------------------------------------------------------- */

#ifndef in_seg7mpx_dig3
  #define in_seg7mpx_dig3

  #include <stdint.h>
  #include <pdk/device.h>
  #include "pfs1xx_gpio.h"

/*

                      12 11 10  9  8  7
                       :  :  :  :  :  :
     a            +-----------------------+
    ---           |   seg3   seg2  seg1   |
 f | g | b        |    --     --    --    |
    ---           |   |  |   |  |  |  |   |
 e |   | c        |    --     --    --    |
    ---           |   |  |   |  |  |  |   |
     d            |    --     --    --    |
                  +-----------------------+
                       :  :  :  :  :  no
                       1  2  3  4  5  pin


    Pin-Nr.:  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9  | 10 | 11 | 12 |
    -------------------------------------------------------------------------------
    Segment   |  e  |  d  |  dp |  c  |  g  | --- |  b  | GX1 | GX2 | f  | a  | GX3


     --------------------------------------------------------

       Anschlussbelegung der 7-Segmentanzeige an den
                        Controller

     PB0..PB6 fuer die 7-Segmentanzeige;
     PB0: Segment a, B6: Segment g
     PB7: Dezimalpunk (DP)
     PA0: MPX Digit0
     PA3: MPX Digit1
     PA6: MPX Digit2
     --------------------------------------------------------
*/

  #define seg7_pin_init()     { PBC= 0xff; PAC = 0xff; }

  #define mpx0_set()        PA0_clr()
  #define mpx0_clr()        PA0_set()
  #define mpx1_set()        PA3_clr()
  #define mpx1_clr()        PA3_set()
  #define mpx2_set()        PA4_clr()
  #define mpx2_clr()        PA4_set()

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
