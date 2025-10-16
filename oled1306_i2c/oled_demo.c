/* ------------------------------------------------------------------
                              oled_demo.c

     Ansteuerung eines OLED Displays 128x64 Pixel, I2C-Interface
     und SSD1306 Displaycontroller.

     Compiler  : SDCC 4.0.3
     MCU       : PFS154 / PFS173

     Pinbelegung I2C
     ---------------

     PA5 = SDA
     PA6 = SCL

     18.12.2020   R. Seelig

   ------------------------------------------------------------------ */

/*
                                                             V   G   S   S
                                                             c   N   C   D
                                                             c   D   L   A
             +----------------+                     +-----------------------------+
         PB4 | 1   |___|   16 | PB3                 |        o   o   o   o        |
         PB5 | 2           15 | PB2                 |                             |
         PB6 | 3           14 | PB1                 |   -----------------------   |
         PB7 | 4  PFS154   13 | PB0                 |  |                       |  |
             -------------                          |  |    SSD1306 - OLED     |  |
         VDD | 5 (1)   (8) 12 | GND                 |  |    128 x 64 Pixel     |  |
         PA7 | 6 (2)   (7) 11 | PA0                 |  |                       |  |
 ICPDA / PA6 | 7 (3)   (6) 10 | PA4                 |  |         I2C           |  |
   Vpp / PA5 | 8 (4)   (5)  9 | PA3 / ICPCK         |   ---+---------------+---   |
             +----------------+                     |      |               |      |
                                                    |      +---------------+      |
                                                    +-----------------------------+

*/

#include <stdint.h>
#include "pdk_init.h"
#include "pfs1xx_gpio.h"
#include "delay.h"

#include "i2c.h"
#include "oled1306_i2c.h"

/* ------------------------------------------------------------
                            putint
     gibt einen Integer dezimal aus. Ist Uebergabe
     "komma" != 0 wird ein "Kommapunkt" mit ausgegeben.

     Bsp.: 12345 wird mit printfkomma == als 123.45 ausgegeben.
     (ermoeglicht Pseudofloatausgaben im Bereich)
   ------------------------------------------------------------ */
void putint(int i, char komma)
{
  typedef enum boolean { FALSE, TRUE }bool_t;

  static int zz[]      = { 10000, 1000, 100, 10 };
  bool_t     not_first = FALSE;

  char       zi;
  int        z, b;

  komma= 5-komma;

  if (!i)
  {
    oled_putchar('0');
  }
  else
  {
    if(i < 0)
    {
      oled_putchar('-');
      i = -i;
    }
    for(zi = 0; zi < 4; zi++)
    {
      z = 0;
      b = 0;

      if  ((zi==komma) && komma)
      {
        if (!not_first) oled_putchar('0');
        oled_putchar('.');
        not_first= TRUE;
      }

      while(z + zz[zi] <= i)
      {
        b++;
        z += zz[zi];
      }
      if(b || not_first)
      {
        oled_putchar('0' + b);
        not_first = TRUE;
      }
      i -= z;
    }
    if (komma== 4) oled_putchar('.');
    oled_putchar('0' + i);
  }
}



/* -------------------------------------------------------
                          M-A-I-N
   ------------------------------------------------------- */
int main(void)
{
  uint16_t cx;

  ssd1306_init();
  clrscr();

  gotoxy(1,0); oled_prints("PADAUK PFS-154");
  gotoxy(0,1); oled_prints("----------------");
  gotoxy(5,2); oled_prints("COUNTER");

  doublechar= 1;
  while(1)
  {
    gotoxy(2,5);
    putint(cx,0);
    cx++;
    delay(500);
  }
}
