/*--------------------------------------------------------
                       button_toggle.c

     Togglen von 4 Tastern als Schalterersatz zum Ein-
     stellen digitaler Logikpegel.

     4 LEDs zeigen Ausgangsstatus an

     Compiler  : SDCC 4.0.3
     MCU       : PFS154

     07.10.2025        R. Seelig

  -------------------------------------------------------- */

#include <stdint.h>
#include "pdk_init.h"
#include "pfs1xx_gpio.h"
#include "delay.h"


#define led0_init()    PB0_output_init()
#define led0_set()     PB0_set()
#define led0_clr()     PB0_clr()

#define led1_init()    PB1_output_init()
#define led1_set()     PB1_set()
#define led1_clr()     PB1_clr()

#define led2_init()    PB2_output_init()
#define led2_set()     PB2_set()
#define led2_clr()     PB2_clr()

#define led3_init()    PB3_output_init()
#define led3_set()     PB3_set()
#define led3_clr()     PB3_clr()

#define out0_init()    PB4_output_init()
#define out0_set()     PB4_set()
#define out0_clr()     PB4_clr()

#define out1_init()    PB5_output_init()
#define out1_set()     PB5_set()
#define out1_clr()     PB5_clr()

#define out2_init()    PB6_output_init()
#define out2_set()     PB6_set()
#define out2_clr()     PB6_clr()

#define out3_init()    PB7_output_init()
#define out3_set()     PB7_set()
#define out3_clr()     PB7_clr()

#define out_inits()    { led0_init(); led1_init(); led2_init(); led3_init();    \
                         out0_init(); out1_init(); out2_init(); out3_init(); }

#define but0_init()    PA7_input_init()
#define is_but0()      !is_PA7()

#define but1_init()    PA6_input_init()
#define is_but1()      !is_PA6()

#define but2_init()    PA5_input_init()
#define is_but2()      !is_PA5()

#define but3_init()    PA3_input_init()
#define is_but3()      !is_PA3()

#define but_inits()    { but0_init(); but1_init(); but2_init(); but3_init(); }

uint8_t out_stat = 0;


/*--------------------------------------------------------
                       stat_send

    sendet das untere Nibble in stat an Leuchdioden und
    Ausgaengen
  -------------------------------------------------------- */
void stat_send(uint8_t stat)
{
  if (stat & 0x01) { out0_set(); led0_set(); }
              else { out0_clr(); led0_clr(); }
  if (stat & 0x02) { out1_set(); led1_set(); }
              else { out1_clr(); led1_clr(); }
  if (stat & 0x04) { out2_set(); led2_set(); }
              else { out2_clr(); led2_clr(); }
  if (stat & 0x08) { out3_set(); led3_set(); }
              else { out3_clr(); led3_clr(); }
}

#define eptime    50           // Prelldauer der Tasten in Millisekunden

/* --------------------------------------------------
                           main
   -------------------------------------------------- */
void main(void)
{
  out_inits();
  but_inits();


  stat_send(out_stat);
  while(1)
  {

    if (is_but0())
    {
      out_stat= out_stat ^ 0x01;
      stat_send(out_stat);
      delay(eptime);
      while(is_but0());                  // warten bis Taste wieder losgelassen wird
      delay(eptime);
    }

    if (is_but1())
    {
      out_stat= out_stat ^ 0x02;
      stat_send(out_stat);
      delay(eptime);
      while(is_but1());                  // warten bis Taste wieder losgelassen wird
      delay(eptime);
    }

    if (is_but2())
    {
      out_stat= out_stat ^ 0x04;
      stat_send(out_stat);
      delay(eptime);
      while(is_but2());                  // warten bis Taste wieder losgelassen wird
      delay(eptime);
    }

    if (is_but3())
    {
      out_stat= out_stat ^ 0x08;
      stat_send(out_stat);
      delay(eptime);
      while(is_but3());                  // warten bis Taste wieder losgelassen wird
      delay(eptime);
    }

  }
}
