/* -------------------------------------------------------
                      txtlcd_demo.c

     Testprogramm fuer ein Standardtextdisplay mit
     HD44780 kompatiblen LC-Controller, 4-Bit Daten-
     transfermodus

     Hardware : Text-LCD

     MCU      :  PFS154
     Takt     :  8 MHz intern

     15.12.2020  R. Seelig

   ------------------------------------------------------- */

/*
      Anschluss Pollin-Display C0802-04 an den Controller
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
// ------------------------------------------------------------------------------------------------------------------


#include <stdint.h>
#include "pdk_init.h"
#include "pfs1xx_gpio.h"
#include "delay.h"

#include "hd44780.h"
#include "my_printf.h"

#define printf    my_printf


static const unsigned char charbmp0[8] =         // hochgestelltes "o" (fuer Gradangaben)
  { 0x0c, 0x12, 0x12, 0x0c, 0x00, 0x00, 0x00, 0x00 };

static const unsigned char charbmp1[8] =         // Pfeil nach links
  { 0x08, 0x0c, 0x0e, 0x0f, 0x0e, 0x0c, 0x08, 0x00 };

static const unsigned char ohmbmp[8] =           // Ohmzeichen
  { 0x0e, 0x11, 0x11, 0x11, 0x0a, 0x0a, 0x1b, 0x00};

/* --------------------------------------------------------
   my_putchar

   wird von my-printf / printf aufgerufen und hier muss
   eine Zeichenausgabefunktion angegeben sein, auf das
   printf dann schreibt !
   -------------------------------------------------------- */
void my_putchar(char ch)
{
  txlcd_putchar(ch);
}


/* -------------------------------------------------------
                          M-A-I-N
   ------------------------------------------------------- */
int main()
{
  uint16_t cx;

  txlcd_init();

  txlcd_setuserchar(0,&charbmp0[0]);
  txlcd_setuserchar(1,&charbmp1[0]);
  txlcd_setuserchar(2,&ohmbmp[0]);

  gotoxy(1,1); printf("UserChar");
  gotoxy(1,2); printf("%c %c %c", 0,1,2);

  delay(2000);
  txlcd_init();
  gotoxy(1,1); printf("Counter");
  cx= 0;
  while(1)
  {
    gotoxy(2,2);
    printf("%d",cx);
    cx++;
    delay(1000);
  }
}
