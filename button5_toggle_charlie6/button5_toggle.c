/* -------------------------------------------------------
                        button5_toggle.c

     Toggelt 5 Ausgangszustaende die ueber Tasten vor-
     genommen werden koennen.

     Der Zustand der Ausgaenge wird ueber charlie-
     geplexte Leuchtdioden angezeigt.

     Compiler  : SDCC 4.0.3
     MCU       : PFS154 / PFS173

     08.10.2025        R. Seelig
   ------------------------------------------------------ */

/*
             +----------------+
         PB4 | 1   |___|   16 | PB3
         PB5 | 2           15 | PB2
         PB6 | 3           14 | PB1
         PB7 | 4  PFS154   13 | PB0
              ----------------
         VDD | 5 (1)   (8) 12 | GND
         PA7 | 6 (2)   (7) 11 | PA0
 ICPDA / PA6 | 7 (3)   (6) 10 | PA4
   Vpp / PA5 | 8 (4)   (5)  9 | PA3 / ICPCK
             +----------------+
*/

#include <stdint.h>
#include "pdk_init.h"
#include "pfs1xx_gpio3.h"

#include "delay.h"
#include "charlie6.h"

/* --------------------------------------------------------
                         Inputs
   -------------------------------------------------------- */
#define keya_init()    PA7_input_init()
#define is_keya()      (!is_PA7())

#define keyb_init()    PA6_input_init()
#define is_keyb()      (!is_PA6())

#define keyc_init()    PA5_input_init()
#define is_keyc()      (!is_PA5())

#define keyd_init()    PA4_input_init()
#define is_keyd()      (!is_PA4())

#define keye_init()    PA3_input_init()
#define is_keye()      (!is_PA3())

#define key_init()     { keya_init(); keyb_init(); keyc_init(); keyd_init(); keye_init(); }

/* --------------------------------------------------------
                         Outputs
   -------------------------------------------------------- */

#define outa_init()    PB4_output_init()
#define outa_set()     PB4_set()
#define outa_clr()     PB4_clr()

#define outb_init()    PB5_output_init()
#define outb_set()     PB5_set()
#define outb_clr()     PB5_clr()

#define outc_init()    PB6_output_init()
#define outc_set()     PB6_set()
#define outc_clr()     PB6_clr()

#define outd_init()    PB7_output_init()
#define outd_set()     PB7_set()
#define outd_clr()     PB7_clr()

#define oute_init()    PB3_output_init()
#define oute_set()     PB3_set()
#define oute_clr()     PB3_clr()

#define binout_init()  { outa_init(); outb_init(); outc_init(); outd_init(); oute_init(); }

uint8_t outstat = 0;

/* --------------------------------------------------------
                       interrupt

     der Interrupt-Handler

     Leider hat sich (scheinbar) ein BUG in SDCC 4.0.3
     bzgl. des Schreibens eines 16-Bit Timerwertes ein-
     geschlichen und deshalb wird das Zaehlerregister
     des Timers mittels Maschinenbefehle beschrieben

     Interrupt wird jede Millisekunde aufgerufen (Reload-
     wert in charlie6.c)

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


/* ------------------------------------------------------

                            main
   ------------------------------------------------------ */
int main(void)
{
  uint8_t cx, i;

  charlie6_init();
  key_init();
  binout_init();
  delay(100);
  outstat= 0;

  while(1)
  {
    if (is_keya())
    {
      charlie6_buf ^= (0x01);
      outstat ^= (0x01);
      if (outstat & 0x01) { outa_set(); } else { outa_clr(); }

      delay(50);
      while(is_keya())
      delay(50);
    }

    if (is_keyb())
    {
      charlie6_buf ^= (0x02);
      outstat ^= (0x02);
      if (outstat & 0x02) { outb_set(); } else { outb_clr(); }

      delay(50);
      while(is_keyb())
      delay(50);
    }

    if (is_keyc())
    {
      charlie6_buf ^= (0x04);
      outstat ^= (0x04);
      if (outstat & 0x04) { outc_set(); } else { outc_clr(); }

      delay(50);
      while(is_keyc())
      delay(50);
    }

    if (is_keyd())
    {
      charlie6_buf ^= (0x08);
      outstat ^= (0x08);
      if (outstat & 0x08) { outd_set(); } else { outd_clr(); }

      delay(50);
      while(is_keyd())
      delay(50);
    }

    if (is_keye())
    {
      charlie6_buf ^= (0x10);
      outstat ^= (0x10);
      if (outstat & 0x10) { oute_set(); } else { oute_clr(); }

      delay(50);
      while(is_keye())
      delay(50);
    }

  }
}
