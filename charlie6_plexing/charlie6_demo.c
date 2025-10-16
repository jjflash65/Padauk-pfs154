/* -------------------------------------------------------
                        charlie6_demo.c

     Demoprogramm fuer Ansteuerung von 6 LED's mittels
     Charlieplexing

     Compiler  : SDCC 4.0.3
     MCU       : PFS154

     19.10.2020        R. Seelig
   ------------------------------------------------------ */

#include <stdint.h>
#include "pdk_init.h"
#include "pfs1xx_gpio.h"

#include "delay.h"
#include "charlie6.h"

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
    charlie6_mpxlines();

    INTRQ &= ~INTRQ_T16;          // Interruptanforderung quittieren
  }
}

#define speed    100

#define bincount 0
/* ------------------------------------------------------
                            main
   ------------------------------------------------------ */
int main(void)
{
  uint8_t  toggleflag = 1;
  uint8_t  cx = 0;

  charlie6_init();

  #if (bincount == 1)
    while(1)
    {
      charlie6_buf= cx;
      delay(speed * 3);
      cx++;
    }
  #else
    charlie6_buf= 1;
    while(1)
    {
      if (toggleflag)
        charlie6_buf = charlie6_buf << 1;
      else
        charlie6_buf = charlie6_buf >> 1;

      delay(speed);

      if ((charlie6_buf== 0x20) && toggleflag) toggleflag ^= 1;
      if ((charlie6_buf== 0x01) && !(toggleflag)) toggleflag ^= 1;
    }
  #endif
}
