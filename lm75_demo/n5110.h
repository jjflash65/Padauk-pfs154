/*--------------------------------------------------------
                           n5110.h

     Header fuer Softwaremodul zum Ansprechen eines
     84x48 LCD (Nokia5110-Display) als Textdisplay.
     Aufgrund sehr geringen Rams ist dieses Display in
     Verbindung mit einem PFS154 Controllers nur als
     Textdisplay verwendbar.

     Es wird ein 5x7 Font verwendet. Der Abstand in
     Y-Achse zwischen den Zeichen betraegt eine Spalte.
     Der Gesamtbedarf eines Zeichens in x-Achse betraegt
     somit 8 Pixel. In y-Achse wird zwischen den Zeichen
     ebenfalls ein Pixel, somit ist der Platzbedarf eines
     Zeichens 6x8 Pixel.

     Dieses entspricht einer Textaufloesung von:
                       14x6 Zeichen

     Compiler  : SDCC 4.0.3
     MCU       : PFS154 / PFS173

     14.10.2020        R. Seelig

  -------------------------------------------------------- */

#ifndef in_N5110
  #define in_N5110

  #include <stdint.h>
  #include "pfs1xx_gpio.h"
  #include "delay.h"

  #define n5110      1                  // Display 84x48 Pixel
  #define n3410      0                  // Display 96x64 Pixel

  #define fullascii  1                  // 1 : Ascii-Zeichen von 32 .. 126 sind verfuegbar
                                        // 0 : nur Zeichen von 32 .. 95 (somit keine kleinen
                                        //     Buchstaben)

  // -----------------------------------------------------
  //  Zuordnung LCD-Anschluesse <=> Controlleranschluesse
  // -----------------------------------------------------

  #define LCD_RST_INIT()      PB0_output_init();
  #define LCD_RST_SET()       PB0_set();
  #define LCD_RST_CLR()       PB0_clr();

  #define LCD_DC_INIT()       PB1_output_init();
  #define LCD_DC_SET()        PB1_set();
  #define LCD_DC_CLR()        PB1_clr();

  #define SPI_MOSI_INIT()     PB2_output_init();
  #define SPI_MOSI_SET()      PB2_set();
  #define SPI_MOSI_CLR()      PB2_clr();

  #define SPI_CLK_INIT()      PB3_output_init();
  #define SPI_CLK_SET()       PB3_set();
  #define SPI_CLK_CLR()       PB3_clr();

  // -----------------------------------------------------
  //  Unterschiedliche Definitionen fuer 3310/ 5510 und
  //  3410 Displays
  // -----------------------------------------------------
  #if (n3410 == 1)
    // Nokia 3410

    #define LCD_VISIBLE_X_RES     96
    #define LCD_VISIBLE_Y_RES     64
    #define LCD_REAL_X_RES        102
    #define LCD_REAL_Y_RES        72
    #define OK                    0
    #define OUT_OF_BORDER         1
  #endif

  #if (n5110 == 1)
    // Nokia 5110 / 3310

    #define LCD_VISIBLE_X_RES     84
    #define LCD_VISIBLE_Y_RES     48
    #define LCD_REAL_X_RES        84
    #define LCD_REAL_Y_RES        48
    #define OK                    0
    #define OUT_OF_BORDER         1
  #endif

  // -----------------------------------------------------
  //                globale Variable
  // -----------------------------------------------------

  extern char wherex;
  extern char wherey;
  extern char invchar;               // = 1 fuer inversive Textausgabe

  // -----------------------------------------------------
  //                  Prototypen
  // -----------------------------------------------------

  void spi_init(void);
  void lcd_init(void);
  void clrscr(void);
  void gotoxy(char x,char y);
  void lcd_putchar(char ch);
  void lcd_puts(char *c);


#endif
