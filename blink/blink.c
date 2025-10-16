/*--------------------------------------------------------
                              blink.c

    erstes Blinky - Demoprogramm fuer den Padauk PFS154
    "Ulta-Low-Cost" Controller.

    Blinkprogramm mit 2 unterschiedlichen Blinkfrequenzen
    die mittels Taster ausgewaehlt werden koennen.

     Compiler  : SDCC 4.0.3
     MCU       : PFS154

     07.10.2020        R. Seelig

  -------------------------------------------------------- */

#include <stdint.h>
#include "pdk_init.h"
#include "pfs1xx_gpio.h"
#include "delay.h"

// LED-Anschlusspin

#define led_init()     PA0_output_init()
#define led_set()      PA0_set()
#define led_clr()      PA0_clr()
#define led_toggle()   PA0_toggle()


#define speed1         1000


/* --------------------------------------------------
                           main
   -------------------------------------------------- */
void main(void)
{

  led_init();

  while(1)
  {
/*
    led_toggle();
    delay(speed1);
*/

    led_clr();
    delay(speed1);
    led_set();
    delay(speed1);

  }
}
