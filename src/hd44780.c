/*   -------------------------------------------------------
                          hd44780.c

     Softwaremodul fuer grundlegende Funktionen eines HD44780
     kompatiblen LC-Textdisplays

     Hardware : Text-LCD

     MCU      :  PFS154
     Takt     :  8 MHz intern

     15.12.2020  R. Seelig

   ------------------------------------------------------- */

/*
      Anschluss am Bsp. Pollin-Display C0802-04
      ---------------------------------------------------
         o +5V
         |                            Display                    PFS154
         _                        Funktion   PIN            PIN    Funktion
        | |
        |_|                          GND      1 ------------
         |                          +5V       2 ------------
         o----o Kontrast   ---    Kontrast    3 ------------
        _|_                           RS      4 ------------  9      PB0
        \ /                          GND      5 ------------
        -'-                    (Takt) E       6 ------------  8      PB1
         |                           D4       7 ------------  2      PB2
        --- GND                      D5       8 ------------  3      PB3
                                     D6       9 ------------  6      PB4
                                     D7      10 ------------  7      PB5

*/

#include "hd44780.h"

uint8_t wherex, wherey;


/* -------------------------------------------------------
     nibbleout

     sendet ein Halbbyte an das LC-Display

     Uebergabe:
         value  : gesamtes Byte
         nibble : 1 => HiByte wird gesendet
                  0 => LoByte wird gesendet
         HILO= 1 => oberen 4 Bits werden gesendet
         HILO= 0 => untere 4 Bits werden gesendet
   ------------------------------------------------------- */
void nibbleout(uint8_t value, uint8_t nibble)
{
  if (nibble)
  {
    if (testbit(value, 7 )) d7_set(); else d7_clr();
    if (testbit(value, 6 )) d6_set(); else d6_clr();
    if (testbit(value, 5 )) d5_set(); else d5_clr();
    if (testbit(value, 4 )) d4_set(); else d4_clr();
  }
  else
  {
    if (testbit(value, 3 )) d7_set(); else d7_clr();
    if (testbit(value, 2 )) d6_set(); else d6_clr();
    if (testbit(value, 1 )) d5_set(); else d5_clr();
    if (testbit(value, 0 )) d4_set(); else d4_clr();
  }

}


/* -------------------------------------------------------
      txlcd_clock

      gibt einen Clockimpuls an das Display
   ------------------------------------------------------- */
void txlcd_clock(void)
{
  e_set();
  _delay_us(60);
  e_clr();
  _delay_us(60);
}

/* -------------------------------------------------------
      txlcd_io

      sendet ein Byte an das Display

      Uebergabe:
         value = zu sendender Wert
   ------------------------------------------------------- */
void txlcd_io(uint8_t value)
{
  nibbleout(value, 1);
  txlcd_clock();
  nibbleout(value, 0);
  txlcd_clock();
}

/* -------------------------------------------------------
     txlcd_init

     initialisiert das Display im 4-Bitmodus
   ------------------------------------------------------- */
void txlcd_init(void)
{
  char i;

  d4_init(); d5_init(); d6_init(); d7_init();
  rs_init(); e_init();

  rs_clr();
  for (i= 0; i< 3; i++)
  {
    txlcd_io(0x20);
    _delay_ms(6);
  }
  txlcd_io(0x28);
  _delay_ms(6);
  txlcd_io(0x0c);
  _delay_ms(6);
  txlcd_io(0x01);
  _delay_ms(6);
  wherex= 0; wherey= 0;
}

/* -------------------------------------------------------
     gotoxy

     setzt den Textcursor an eine Stelle im Display. Die
     obere linke Ecke hat die Koordinate (1,1)
   ------------------------------------------------------- */
void gotoxy(uint8_t x, uint8_t y)
{
  uint8_t txlcd_adr;

  txlcd_adr= (0x80+((y-1)*0x40))+x-1;
  rs_clr();
  txlcd_io(txlcd_adr);
  wherex= x;
  wherey= y;
}

/* -------------------------------------------------------
     txlcd_setuserchar

     kopiert die Bitmap eines benutzerdefiniertes Zeichen
     in den Charactergenerator des Displaycontrollers

               nr : Position im Ram des Displays, an
                    der die Bitmap hinterlegt werden
                    soll.
        *userchar : Zeiger auf die Bitmap des Zeichens

   Bsp.:  txlcd_setuserchar(3,&meinezeichen[0]);
          txlcd_putchar(3);

   ------------------------------------------------------- */
void txlcd_setuserchar(uint8_t nr, const uint8_t *userchar)
{
  uint8_t b;

  rs_clr();
  txlcd_io(0x40+(nr << 3));                         // CG-Ram Adresse fuer eigenes Zeichen
  rs_set();
  for (b= 0; b< 8; b++)
  {
    txlcd_io(*userchar);
    userchar++;
  }
  rs_clr();
}


/* -------------------------------------------------------
     txlcd_putchar

     gibt ein Zeichen auf dem Display aus

     Uebergabe:
         ch = auszugebendes Zeichen
   ------------------------------------------------------- */

void txlcd_putchar(char ch)
{
  rs_set();
  txlcd_io(ch);
  wherex++;
}

/* -------------------------------------------------------
      txlcd_prints

      gibt einen AsciiZ Text der im RAM gespeichert ist
      auf dem Display aus.

      Bsp.:

      char strbuf[] = "H. Welt";

      txlcd_prints(strbuf);
   ------------------------------------------------------- */

void txlcd_prints(uint8_t *c)
{
  while (*c)
  {
    txlcd_putchar(*c++);
  }
}

