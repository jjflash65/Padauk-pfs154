/*--------------------------------------------------------
                        lauflicht.c


     Compiler  : SDCC 4.0.3
     MCU       : PFS154

     07.10.2020        R. Seelig

  -------------------------------------------------------- */

#include <stdint.h>
#include "pdk_init.h"
#include "pfs1xx_gpio.h"
#include "delay.h"

// LED-Ausgabe auf PortB, gemeinsame Anode an +Vcc

#define ledband_init()           ( PBC= 0xff )
#define ledband_set(value)       { PB= value; }


#define speed         100


/* --------------------------------------------------
                           main
   -------------------------------------------------- */
void main(void)
{
  uint8_t outval;

  ledband_init();

  outval= 1;
  while(1)
  {
    while(outval)
    {
      ledband_set(~outval);
      delay(speed);
      outval = outval << 1;
    }
    outval= 0x40;
    while(outval)
    {
      ledband_set(~outval);
      delay(speed);
      outval = outval >> 1;
    }
    outval= 0x02;
  }
}
