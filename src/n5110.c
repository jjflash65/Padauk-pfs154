/*--------------------------------------------------------
                           n5110.c

     Softwaremodul zum Ansprechen eines 84x48 LCD
     (Nokia5110-Display) als Textdisplay. Aufgrund sehr
     geringen Rams ist dieses Display in Verbindung mit
     einem PFS154 Controllers nur als Textdisplay
     verwendbar.

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

#include "n5110.h"

char wherex= 0;
char wherey= 0;
char invchar= 0;               // = 1 fuer inversive Textausgabe


#if (fullascii == 1)
  #define lastascii    126
#else
  #define lastascii    95
#endif

extern const uint8_t fonttab [][5];


/* -------------------------------------------------------------
                          spi_init

     Pins des Software-SPI als Ausgaenge konfigurieren
   ------------------------------------------------------------- */
void spi_init(void)
{
  SPI_MOSI_INIT();

  SPI_CLK_INIT();
  SPI_CLK_CLR();
}

/* -------------------------------------------------------------
                           spi_out

      Byte ueber Software SPI senden / empfangen
      data ==> zu sendendes Datum
   ------------------------------------------------------------- */
void spi_out(uint8_t data)
{
  char a;

  for (a=0;a<8;a++)
  {
    if((data & 0x80)> 0) { SPI_MOSI_SET(); }
                    else { SPI_MOSI_CLR(); }

    // Taktimpuls erzeugen
    SPI_CLK_SET();                          // Taktleitung = 1
    SPI_CLK_CLR();                          // Taktleitung = 0

    data <<= 1;
  }
}

/* -------------------------------------------------------------
                              wrcmd

   sendet Kommando via SPI an das LCD
   ------------------------------------------------------------- */
void wrcmd(uint8_t cmd)
{
  LCD_DC_CLR();                           // C/D = 0 --> Command-Mode
  spi_out(cmd);                           // senden
}

/* -------------------------------------------------------------
                              wrdata

   sendet Datum via SPI an das LCD
   ------------------------------------------------------------- */
void wrdata(uint8_t data)
{
  LCD_DC_SET();                          // C/D = 1 --> Data-Mode
  spi_out(data);                         // senden
}

/* -------------------------------------------------------------
                             lcd_init

   initialisiert das Display

   Register PCD8544
   Functionset: 0x20 + DB0..DB2
   ----------------------------------------------------
                  0          |      1
     DB0: basic instruction  |   extended
     DB1: horizontal addr    |   vertical addr
     DB2: diplay active      |   powerdown


   Display-ctrl: 0x08 + DB2 + DB0
   ----------------------------------------------------
       DB2  |  DB0
        0       0    : display blank
        0       1    : all segments on
        1       0    : normal mode
        1       1    : inverted mode

   Temperature: 0x04 + DB0 + DB1
   ----------------------------------------------------
       DB0..DB1 => Temperaturecoeff 0..3


   VOP (contrast voltage  LC-Display) : 0x80 + DB0..DB5
   ----------------------------------------------------
       DB0..DB5 => voltage to the Display

   BIAS : 0x10 + DB0..DB2
   ----------------------------------------------------

   ------------------------------------------------------------- */
void lcd_init(void)
{
  // Pin - Init

  LCD_RST_INIT();
  LCD_DC_INIT();

  LCD_RST_CLR();                // Reset LCD controller
  delay(1);
  LCD_RST_SET();                // LCD "unreset"

  spi_init();

  // LCD Controller Init

  wrcmd(0x21); _delay_us(5);
  wrcmd(0x09); _delay_us(5);
  wrcmd(0xc8); _delay_us(5);
  wrcmd(0x10); _delay_us(5);
  wrcmd(0x04); _delay_us(5);
  wrcmd(0x20); _delay_us(5);
  wrcmd(0x0c); delay(20);

  clrscr();
}

/* -----------------------------------------------------
                            clrscr

   loescht den Displayinhalt
   ----------------------------------------------------- */
void clrscr(void)
{
  int  i=0;

  wrcmd(0x80);             // Anfangsadresse des Displays
  wrcmd(0x40);
  for (i= 1; i< (LCD_REAL_X_RES * LCD_REAL_Y_RES/8)+1; i++) { wrdata(0x00); }
  gotoxy(0,0);
}

/* -----------------------------------------------------
                            gotoxy

   positioniert die Textausgabeposition auf X/Y
   ----------------------------------------------------- */
void gotoxy(char x,char y)
{
  wrcmd(0x80+(x*6));
  wrcmd(0x40+y);
  wherex= x; wherey= y;
}

/* -----------------------------------------------------
                          putchar

   gibt ein Zeichen auf dem Display aus
   (die Steuerkommandos \n und \r fuer <printf> sind
   implementiert)
   ----------------------------------------------------- */
void lcd_putchar(char ch)
{
  uint8_t b,rb;

  if (ch== 13)
  {
    gotoxy(0,wherey);
    return;
  }
  if (ch== 10)
  {
    wherey++;
    gotoxy(wherex,wherey);
    return;
  }

  if (ch== 8)
  {
    if ((wherex> 0))
    {
      wherex--;
      gotoxy(wherex,wherey);
      for (b= 0;b<6;b++)
      {
        if (invchar) {wrdata(0xff);} else {wrdata(0);}
      }
      gotoxy(wherex,wherey);
    }
    return;
  }

  if ((ch<0x20)||(ch>lastascii)) ch = 92;               // ASCII Zeichen umrechnen

  // Kopiere Daten eines Zeichens aus dem Zeichenarray in den LCD-Screenspeicher

  for (b= 0; b<5; b++)
  {
    rb= fonttab[ch-32][b];
    if (invchar) {rb= ~rb;}
    wrdata(rb);
  }
  if (invchar) {wrdata(0xff);} else {wrdata(0);}
  wherex++;
  if (wherex> 15)
  {
    wherex= 0;
    wherey++;
  }
  gotoxy(wherex,wherey);
}

/* ---------------------------------------------------
                         lcd_puts
   gibt einen Text an der aktuellen Position aus

   Uebergabe:
              *c : Zeiger auf einen String
   ---------------------------------------------------*/
void lcd_puts(char *c)
{
  while (*c)
  {
    lcd_putchar(*c++);
  }
}

// --------------------------------------------------------------
// Zeichensatz
// --------------------------------------------------------------


/* Bitmaps des Ascii-Zeichensatzes
   ein Smily wuerde so aussehen:
      { 0x36, 0x46, 0x40, 0x46, 0x36 }  // Smiley

   ein grosses A ist folgendermassen definiert:

   { 0x7E, 0x11, 0x11, 0x11, 0x7E }

   . x x x x x x .
   . . . x . . . x
   . . . x . . . x
   . . . x . . . x
   . x x x x x x .

*/

// --------------------------------------------------------------
// Zeichensatz
// --------------------------------------------------------------


/* Bitmaps des Ascii-Zeichensatzes
   ein Smily wuerde so aussehen:
      { 0x36, 0x46, 0x40, 0x46, 0x36 }  // Smiley

   ein grosses A ist folgendermassen definiert:

   { 0x7E, 0x11, 0x11, 0x11, 0x7E }

   . x x x x x x .
   . . . x . . . x
   . . . x . . . x
   . . . x . . . x
   . x x x x x x .

*/

const uint8_t fonttab [][5] = {
    { 0x00, 0x00, 0x00, 0x00, 0x00 },   // space
    { 0x00, 0x00, 0x2f, 0x00, 0x00 },   // !
    { 0x00, 0x07, 0x00, 0x07, 0x00 },   // "
    { 0x14, 0x7f, 0x14, 0x7f, 0x14 },   // #
    { 0x24, 0x2a, 0x7f, 0x2a, 0x12 },   // $
    { 0xc4, 0xc8, 0x10, 0x26, 0x46 },   // %
    { 0x36, 0x49, 0x55, 0x22, 0x50 },   // &
    { 0x00, 0x05, 0x03, 0x00, 0x00 },   // '
    { 0x00, 0x1c, 0x22, 0x41, 0x00 },   // (
    { 0x00, 0x41, 0x22, 0x1c, 0x00 },   // )
    { 0x14, 0x08, 0x3E, 0x08, 0x14 },   // *
    { 0x08, 0x08, 0x3E, 0x08, 0x08 },   // +
    { 0x00, 0x00, 0x50, 0x30, 0x00 },   // ,
    { 0x10, 0x10, 0x10, 0x10, 0x10 },   // -
    { 0x00, 0x60, 0x60, 0x00, 0x00 },   // .
    { 0x20, 0x10, 0x08, 0x04, 0x02 },   // /
    { 0x3E, 0x51, 0x49, 0x45, 0x3E },   // 0
    { 0x00, 0x42, 0x7F, 0x40, 0x00 },   // 1
    { 0x42, 0x61, 0x51, 0x49, 0x46 },   // 2
    { 0x21, 0x41, 0x45, 0x4B, 0x31 },   // 3
    { 0x18, 0x14, 0x12, 0x7F, 0x10 },   // 4
    { 0x27, 0x45, 0x45, 0x45, 0x39 },   // 5
    { 0x3C, 0x4A, 0x49, 0x49, 0x30 },   // 6
    { 0x01, 0x71, 0x09, 0x05, 0x03 },   // 7
    { 0x36, 0x49, 0x49, 0x49, 0x36 },   // 8
    { 0x06, 0x49, 0x49, 0x29, 0x1E },   // 9
    { 0x00, 0x36, 0x36, 0x00, 0x00 },   // :
    { 0x00, 0x56, 0x36, 0x00, 0x00 },   // ;
    { 0x08, 0x14, 0x22, 0x41, 0x00 },   // <
    { 0x14, 0x14, 0x14, 0x14, 0x14 },   // =
    { 0x00, 0x41, 0x22, 0x14, 0x08 },   // >
    { 0x02, 0x01, 0x51, 0x09, 0x06 },   // ?
    { 0x32, 0x49, 0x59, 0x51, 0x3E },   // @
    { 0x7E, 0x11, 0x11, 0x11, 0x7E },   // A
    { 0x7F, 0x49, 0x49, 0x49, 0x36 },   // B
    { 0x3E, 0x41, 0x41, 0x41, 0x22 },   // C
    { 0x7F, 0x41, 0x41, 0x22, 0x1C },   // D
    { 0x7F, 0x49, 0x49, 0x49, 0x41 },   // E
    { 0x7F, 0x09, 0x09, 0x09, 0x01 },   // F
    { 0x3E, 0x41, 0x49, 0x49, 0x7A },   // G
    { 0x7F, 0x08, 0x08, 0x08, 0x7F },   // H
    { 0x00, 0x41, 0x7F, 0x41, 0x00 },   // I
    { 0x20, 0x40, 0x41, 0x3F, 0x01 },   // J
    { 0x7F, 0x08, 0x14, 0x22, 0x41 },   // K
    { 0x7F, 0x40, 0x40, 0x40, 0x40 },   // L
    { 0x7F, 0x02, 0x0C, 0x02, 0x7F },   // M
    { 0x7F, 0x04, 0x08, 0x10, 0x7F },   // N
    { 0x3E, 0x41, 0x41, 0x41, 0x3E },   // O
    { 0x7F, 0x09, 0x09, 0x09, 0x06 },   // P
    { 0x3E, 0x41, 0x51, 0x21, 0x5E },   // Q
    { 0x7F, 0x09, 0x19, 0x29, 0x46 },   // R
    { 0x46, 0x49, 0x49, 0x49, 0x31 },   // S
    { 0x01, 0x01, 0x7F, 0x01, 0x01 },   // T
    { 0x3F, 0x40, 0x40, 0x40, 0x3F },   // U
    { 0x1F, 0x20, 0x40, 0x20, 0x1F },   // V
    { 0x3F, 0x40, 0x38, 0x40, 0x3F },   // W
    { 0x63, 0x14, 0x08, 0x14, 0x63 },   // X
    { 0x07, 0x08, 0x70, 0x08, 0x07 },   // Y
    { 0x61, 0x51, 0x49, 0x45, 0x43 },   // Z
    { 0x00, 0x7F, 0x41, 0x41, 0x00 },   // [
    { 0x55, 0x2A, 0x55, 0x2A, 0x55 },   // "Yen"
    { 0x00, 0x41, 0x41, 0x7F, 0x00 },   // ]
    { 0x04, 0x02, 0x01, 0x02, 0x04 },   // ^
    { 0x40, 0x40, 0x40, 0x40, 0x40 },   // _
    { 0x00, 0x01, 0x02, 0x04, 0x00 },   // '

#if (fullascii == 1)
    { 0x20, 0x54, 0x54, 0x54, 0x78 },   // a
    { 0x7F, 0x48, 0x44, 0x44, 0x38 },   // b
    { 0x38, 0x44, 0x44, 0x44, 0x20 },   // c
    { 0x38, 0x44, 0x44, 0x48, 0x7F },   // d
    { 0x38, 0x54, 0x54, 0x54, 0x18 },   // e
    { 0x08, 0x7E, 0x09, 0x01, 0x02 },   // f
    { 0x0C, 0x52, 0x52, 0x52, 0x3E },   // g
    { 0x7F, 0x08, 0x04, 0x04, 0x78 },   // h
    { 0x00, 0x44, 0x7D, 0x40, 0x00 },   // i
    { 0x20, 0x40, 0x44, 0x3D, 0x00 },   // j
    { 0x7F, 0x10, 0x28, 0x44, 0x00 },   // k
    { 0x00, 0x41, 0x7F, 0x40, 0x00 },   // l
    { 0x7C, 0x04, 0x18, 0x04, 0x78 },   // m
    { 0x7C, 0x08, 0x04, 0x04, 0x78 },   // n
    { 0x38, 0x44, 0x44, 0x44, 0x38 },   // o
    { 0x7C, 0x14, 0x14, 0x14, 0x08 },   // p
    { 0x08, 0x14, 0x14, 0x18, 0x7C },   // q
    { 0x7C, 0x08, 0x04, 0x04, 0x08 },   // r
    { 0x48, 0x54, 0x54, 0x54, 0x20 },   // s
    { 0x04, 0x3F, 0x44, 0x40, 0x20 },   // t
    { 0x3C, 0x40, 0x40, 0x20, 0x7C },   // u
    { 0x1C, 0x20, 0x40, 0x20, 0x1C },   // v
    { 0x3C, 0x40, 0x30, 0x40, 0x3C },   // w
    { 0x44, 0x28, 0x10, 0x28, 0x44 },   // x
    { 0x0C, 0x50, 0x50, 0x50, 0x3C },   // y
    { 0x44, 0x64, 0x54, 0x4C, 0x44 },   // z
    // Zeichen vom Ascii-Satz abweichend
    { 0x3E, 0x7F, 0x7F, 0x3E, 0x00 },   // Zeichen 123 : ausgefuelltes Oval
    { 0x06, 0x09, 0x09, 0x06, 0x00 },   // Zeichen 124 : hochgestelltes kleines o (fuer Gradzeichen);
    { 0x01, 0x01, 0x01, 0x01, 0x01 },   // Zeichen 125 : Strich in der obersten Reihe
    { 0x00, 0x1D, 0x15, 0x17, 0x00 }    // Zeichen 126 : "hoch 2"
#endif
};


