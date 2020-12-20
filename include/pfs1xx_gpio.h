/* ---------------------------------------------------------
                      pfs1xx_gpio.h

     Makros und Defines zum einfacheren Umgang mit  GPIO -
     pins

     MCU : PFS154 / PFS173

     06.10.2020  R. Seelig
   --------------------------------------------------------- */

#ifndef in_pfsgpio
  #define in_pfsgpio

  #include <pdk/device.h>

  // Bitmasken
  // ----------------------------------------------
  #define MASK0                ( 1 << 0 )
  #define MASK1                ( 1 << 1 )
  #define MASK2                ( 1 << 2 )
  #define MASK3                ( 1 << 3 )
  #define MASK4                ( 1 << 4 )
  #define MASK5                ( 1 << 5 )
  #define MASK6                ( 1 << 6 )
  #define MASK7                ( 1 << 7 )

  extern uint8_t PADIER_TMP;
  extern uint8_t PBDIER_TMP;


  // Macros zur Steuerung der Portbits. Wird ein Portpin als Eingang initialisiert,
  // so hierbei auch der interne Pull-Up Widerstand aktiviert
  /* -----------------------------------------------
                         Port-A
     ----------------------------------------------- */
  #define PA0_output_init()    ( PAC |= MASK0 )
  #define PA0_set()            ( PA  |= MASK0 )
  #define PA0_clr()            ( PA  &= ~MASK0 )
  #define PA0_toggle()         ( PA  ^= MASK0 )

  #define PA0_input_init()     { PAC &= ~MASK0; PADIER_TMP |= MASK0; PADIER= PADIER_TMP; PAPH |= MASK0; }
  #define is_PA0()             ( ((PA & MASK0) >> 0) )

  #define PA1_output_init()    ( PAC |= MASK1 )
  #define PA1_set()            ( PA  |= MASK1 )
  #define PA1_clr()            ( PA  &= ~MASK1 )
  #define PA1_toggle()         ( PA  ^= MASK1 )

  #define PA1_input_init()     { PAC &= ~MASK1; PADIER_TMP |= MASK1; PADIER= PADIER_TMP; PAPH |= MASK1; }
  #define is_PA1()             ( ((PA & MASK1) >> 1) )

  #define PA2_output_init()    ( PAC |= MASK2 )
  #define PA2_set()            ( PA  |= MASK2 )
  #define PA2_clr()            ( PA  &= ~MASK2 )
  #define PA2_toggle()         ( PA  ^= MASK2 )

  #define PA2_input_init()     { PAC &= ~MASK2; PADIER_TMP |= MASK2;  PADIER= PADIER_TMP; PAPH |= MASK2; }
  #define is_PA2()             ( ((PA & MASK2) >> 2) )

  #define PA3_output_init()    ( PAC |= MASK3 )
  #define PA3_set()            ( PA  |= MASK3 )
  #define PA3_clr()            ( PA  &= ~MASK3 )
  #define PA3_toggle()         ( PA  ^= MASK3 )

  #define PA3_input_init()     { PAC &= ~MASK3; PADIER_TMP |= MASK3;  PADIER= PADIER_TMP; PAPH |= MASK3; }
  #define is_PA3()             ( ((PA & MASK3) >> 3) )

  #define PA4_output_init()    ( PAC |= MASK4 )
  #define PA4_set()            ( PA  |= MASK4 )
  #define PA4_clr()            ( PA  &= ~MASK4 )
  #define PA4_toggle()         ( PA  ^= MASK4 )

  #define PA4_input_init()     { PAC &= ~MASK4; PADIER_TMP |= MASK4;  PADIER= PADIER_TMP; PAPH |= MASK4; }
  #define is_PA4()             ( ((PA & MASK4) >> 4) )

  #define PA5_output_init()    ( PAC |= MASK5 )
  #define PA5_set()            ( PA  |= MASK5 )
  #define PA5_clr()            ( PA  &= ~MASK5 )
  #define PA5_toggle()         ( PA  ^= MASK5 )

  #define PA5_input_init()     { PAC &= ~MASK5; PADIER_TMP |= MASK5;  PADIER= PADIER_TMP; PAPH |= MASK5; }
  #define is_PA5()             ( ((PA & MASK5) >> 5) )

  #define PA6_output_init()    ( PAC |= MASK6 )
  #define PA6_set()            ( PA  |= MASK6 )
  #define PA6_clr()            ( PA  &= ~MASK6 )
  #define PA6_toggle()         ( PA  ^= MASK6 )

  #define PA6_input_init()     { PAC &= ~MASK6; PADIER_TMP |= MASK6;  PADIER= PADIER_TMP; PAPH |= MASK6; }
  #define is_PA6()             ( ((PA & MASK6) >> 6) )

  #define PA7_output_init()    ( PAC |= MASK7 )
  #define PA7_set()            ( PA  |= MASK7 )
  #define PA7_clr()            ( PA  &= ~MASK7 )
  #define PA7_toggle()         ( PA  ^= MASK7 )

  #define PA7_input_init()     { PAC &= ~MASK7; PADIER_TMP |= MASK7;  PADIER= PADIER_TMP; PAPH |= MASK7; }
  #define is_PA7()             ( ((PA & MASK7) >> 7) )

  /* -----------------------------------------------
                         Port-B
     ----------------------------------------------- */
  #define PB0_output_init()    ( PBC |= MASK0 )
  #define PB0_set()            ( PB  |= MASK0 )
  #define PB0_clr()            ( PB  &= ~MASK0 )
  #define PB0_toggle()         ( PB  ^= MASK0 )

  #define PB0_input_init()     { PBC &= ~MASK0; PBDIER_TMP |= MASK0;  PBDIER= PBDIER_TMP; PBPH |= MASK0; }
  #define is_PB0()             ( ((PB & MASK0) >> 0) )

  #define PB1_output_init()    ( PBC |= MASK1 )
  #define PB1_set()            ( PB  |= MASK1 )
  #define PB1_clr()            ( PB  &= ~MASK1 )
  #define PB1_toggle()         ( PB  ^= MASK1 )

  #define PB1_input_init()     { PBC &= ~MASK1; PBDIER_TMP |= MASK1;  PBDIER= PBDIER_TMP; PBPH |= MASK1; }
  #define is_PB1()             ( ((PB & MASK1) >> 1) )

  #define PB2_output_init()    ( PBC |= MASK2 )
  #define PB2_set()            ( PB  |= MASK2 )
  #define PB2_clr()            ( PB  &= ~MASK2 )
  #define PB2_toggle()         ( PB  ^= MASK2 )

  #define PB2_input_init()     { PBC &= ~MASK2; PBDIER_TMP |= MASK2;  PBDIER= PBDIER_TMP; PBPH |= MASK2; }
  #define is_PB2()             ( ((PB & MASK2) >> 2) )

  #define PB3_output_init()    ( PBC |= MASK3 )
  #define PB3_set()            ( PB  |= MASK3 )
  #define PB3_clr()            ( PB  &= ~MASK3 )
  #define PB3_toggle()         ( PB  ^= MASK3 )

  #define PB3_input_init()     { PBC &= ~MASK3; PBDIER_TMP |= MASK3;  PBDIER= PBDIER_TMP; PBPH |= MASK3; }
  #define is_PB3()             ( ((PB & MASK3) >> 3) )

  #define PB4_output_init()    ( PBC |= MASK4 )
  #define PB4_set()            ( PB  |= MASK4 )
  #define PB4_clr()            ( PB  &= ~MASK4 )
  #define PB4_toggle()         ( PB  ^= MASK4 )

  #define PB4_input_init()     { PBC &= ~MASK4; PBDIER_TMP |= MASK4;  PBDIER= PBDIER_TMP; PBPH |= MASK4; }
  #define is_PB4()             ( ((PB & MASK4) >> 4) )

  #define PB5_output_init()    ( PBC |= MASK5 )
  #define PB5_set()            ( PB  |= MASK5 )
  #define PB5_clr()            ( PB  &= ~MASK5 )
  #define PB5_toggle()         ( PB  ^= MASK5 )

  #define PB5_input_init()     { PBC &= ~MASK5; PBDIER_TMP |= MASK5;  PBDIER= PBDIER_TMP; PBPH |= MASK5; }
  #define is_PB5()             ( ((PB & MASK5) >> 5) )

  #define PB6_output_init()    ( PBC |= MASK6 )
  #define PB6_set()            ( PB  |= MASK6 )
  #define PB6_clr()            ( PB  &= ~MASK6 )
  #define PB6_toggle()         ( PB  ^= MASK6 )

  #define PB6_input_init()     { PBC &= ~MASK6; PBDIER_TMP |= MASK6;  PBDIER= PBDIER_TMP; PBPH |= MASK6; }
  #define is_PB6()             ( ((PB & MASK6) >> 6) )

  #define PB7_output_init()    ( PBC |= MASK7 )
  #define PB7_set()            ( PB  |= MASK7 )
  #define PB7_clr()            ( PB  &= ~MASK7 )
  #define PB7_toggle()         ( PB  ^= MASK7 )

  #define PB7_input_init()     { PBC &= ~MASK7; PBDIER_TMP |= MASK7;  PBDIER= PBDIER_TMP; PBPH |= MASK7; }
  #define is_PB7()             ( ((PB & MASK7) >> 7) )


#endif
