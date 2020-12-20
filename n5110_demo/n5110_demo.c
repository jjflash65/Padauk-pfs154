/*--------------------------------------------------------
                          n5110_demo.c

     Demo fuer N5110 Monochrom-LCD 84x48

     Compiler  : SDCC 4.0.3
     MCU       : PFS154 / PFS173

     14.10.2020        R. Seelig

  -------------------------------------------------------- */

#include <stdint.h>
#include "pdk_init.h"
#include "pfs1xx_gpio.h"
#include "delay.h"
#include "n5110.h"

#include "my_printf.h"

#define printf         my_printf     // "Umleitung" von printf auf my_printf, damit
                                     // im Quelltext wie gewohnt mittels printf
                                     // Ausgaben gemacht werden koennen

/* --------------------------------------------------------
   my_putchar

   wird von my-printf / printf aufgerufen und hier muss
   eine Zeichenausgabefunktion angegeben sein, auf das
   printf dann schreibt !
   -------------------------------------------------------- */
void my_putchar(uint8_t ch)
{
  lcd_putchar(ch);
}


/* --------------------------------------------------
                           main
   -------------------------------------------------- */
void main(void)
{
  uint16_t cx;

  delay(150);
  lcd_init();
  delay(10);
  clrscr();

  gotoxy(0,0); printf("Padauk PFS154");
  gotoxy(1,1); printf("16-Bit count");
  gotoxy(0,2); printf("--------------");
  cx= 0;
  gotoxy(0,4); printf("Counter:");
  while(1)
  {
    gotoxy(9,4); printf("%d",cx);
    cx++;
    delay(500);
  }
}
