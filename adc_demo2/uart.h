/* ------------------------------------------------------
                         uart.h

     Header fuer Softwaremodul einer Bitbanging er-
     zeugten seriellen Schnittstelle

     Compiler  : SDCC 4.0.3
     MCU       : PFS154 / PFS173

     hier verfuegbare Stringausgabe  kann mittels
     Wertzuweisung an

                    uart_puts_enable

     an- bzw. abgeschaltet werden. Immer dann
     empfehlenswert wenn der Speicherplatz knapp wird !!

     06.10.2020        R. Seelig

  ------------------------------------------------------- */

#ifndef in_uart
  #define in_uart

  #include <pdk/device.h>
  #include "pfs1xx_gpio.h"


  #define uart_txbit        PA5              // Anschlusspin der TX-Leitung
  #define uart_rxbit        PA6              // Anschlusspin der RX-Leitung

  #define uart_puts_enable  1                // 0 : nicht verfuegbar
                                             // 1 : verfuegbar

  /* ---------------------------------------------------------------------------
                                 Baudratentabelle

      Die Implementierung der seriellen Schnittstelle auf dem PFS154 wird
      mittels Bitbanging realisiert. Anstelle eine sehr grosse Macro-Kette
      im Quellcode zu haben gibt es hier eine Tabelle fuer die Konstante
      uart_delfak, die die Baudrate der Schnittstelle fuer unterschiedliche
      Coretakte einstellt (Werte wurden impirisch mittels Logicanalyzer er-
      mitteln):

                           Werte fuer uart_delfak

      F_CPU   | 2400 Bd | 4800 Bd | 9600 Bd | 19200 Bd | 38400 Bd |  57600 Bd | 115200 Bd |
      -------------------------------------------------------------------------------------
         8    |   182   |   88    |   44    |    22    |    10    |     6     |
        16    |         |  182    |   88    |    44    |    22    |    14     |     6
  */

//  #define uart_delfak    141
//  #define uart_delfak    70

    #define uart_delfak    22


  /* --------------------------------------------------
                       Prototypen
     -------------------------------------------------- */

  void uart_init(void);
  void uart_putchar (char c);
  char uart_getchar(void);
  #if ( uart_puts_enable == 1 )

    void uart_puts(char *p);

  #endif


  // ----------------------------------------------------------------
  // Praeprozessormacros um 2 Stringtexte zur weiteren Verwendung
  // innerhalb des Praeprozessors  zu verknuepfen
  //
  // Bsp.:
  //        #define ionr      3
  //        #define ioport    conc2(PA, ionr)
  //
  //        ioport wird nun als "PA3" behandelt
  #define CONC2EXP(a,b)     a ## b
  #define conc2(a,b)        CONC2EXP(a, b)
  // ----------------------------------------------------------------

  // define-Stringtexte der Anschlusspins erzeugen

  #define uarttx_set()    conc2(uart_txbit,_set())
  #define uarttx_clr()    conc2(uart_txbit,_clr())
  #define uarttx_init()   { conc2(uart_txbit,_output_init()); uarttx_set(); }

  #define uartrx_init()   conc2(uart_rxbit,_input_init())
  #define is_uartrx()     conc2(is_,uart_rxbit())

#endif

