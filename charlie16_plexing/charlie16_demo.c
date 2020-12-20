/* -------------------------------------------------------
                        charlie_demo.c

     Demoprogramm fuer Ansteuerung von 20 LED's mittels
     Charlieplexing

     Compiler  : SDCC 4.0.3
     MCU       : PFS154

     19.10.2020        R. Seelig
   ------------------------------------------------------ */

#include <stdint.h>

#include <stdint.h>
#include "pdk_init.h"
#include "pfs1xx_gpio.h"

#include "delay.h"
#include "charlie16.h"



/* --------------------------------------------------------
                       interrupt

     der Interrupt-Handler

     Leider hat sich (scheinbar) ein BUG in SDCC 4.0.3
     bzgl. des Schreibens eines 16-Bit Timerwertes ein-
     geschlichen und deshalb wird das Zaehlerregister
     des Timers mittels Maschinenbefehle beschrieben

     Interrupt wird jede Millisekunde aufgerufen (Reload-
     wert in charlie20.c)

   -------------------------------------------------------- */
void interrupt(void) __interrupt(0)
{

  if (INTRQ & INTRQ_T16)
  {

    // Reloadwert des 16-Bit Timers setzen
    __asm

      mov a,_reload+0
      mov __t16c+0,a
      mov a,_reload+1
      mov __t16c+1,a

      stt16 __t16c
    __endasm;

    // Aufruf der einzelnen Charlieplexing-Linien
    charlie16_mpxlines();

    INTRQ &= ~INTRQ_T16;          // Interruptanforderung quittieren
  }
}



/* ------------------------------------------------------
                            main
   ------------------------------------------------------ */
int main(void)
{
  uint16_t i;

  charlie16_init();

  while(1)
  {
    charlie16_buf= 1;
    for (i= 0; i< 15; i++)
    {
      delay(300);
      charlie16_buf = charlie16_buf << 1;
    }
  }
}
