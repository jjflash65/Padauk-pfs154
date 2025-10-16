/*--------------------------------------------------------
                        timer2_blink.c

     Blinken mittels 8-Bit Timer2

     Compiler  : SDCC 4.0.3
     MCU       : PFS154

     17.12.2020        R. Seelig

  -------------------------------------------------------- */

#include <stdint.h>
#include "pdk_init.h"
#include "pfs1xx_gpio.h"

// LED-Anschlusspin
#define led_init()     PA0_output_init()
#define led_toggle()   PA0_toggle()


/* --------------------------------------------------------
                          tim2_init

     Beteiligte Register: TM2C (timer2 control register)
                          TM2C (timer2 counter register)
                          TM2S (timer2 scalar reigster)

     siehe timer_2_3_reg.txt

     erzeugt einen Interrupt alle 32.768 ms
   -------------------------------------------------------- */
void tim2_init(void)
{
  // 0001     00                    00                         = 0x10
  // SYSCLK   output sel. disable   periode mode / no invers

  TM2C = 0x10;

  // 0        11          11111            = 0x7f
  // PWM-8    prescale    clock divisior

  // nach prescaler:  F_CPU(8 MHz) /64 (prescale)       => F_PRE = 125 kHz
  // Taktfrequenz:    F_PRE / 32 (scale) = 3.90625 kHz  => t= 0.256 ms
  TM2S = 0x7f;

  // Boundaryregister (wie compare match), ausloesen des Interrupts
  // nach 0x80 = 128d Zaehlimpulsen
  // Interruptrequest nach: t * 128 = 32.768 ms
  TM2B = 0x80;


  __engint();                   // grundsaetzlich Interrupt zulassen
  INTEN |= INTEN_TM2;           // Timerinterrupt zulassen
  TM2CT= 0x00;                  // Counter auf 0 setzen
}

/* --------------------------------------------------------
                       interrupt

     der Interrupt-Handler. Innerhalb dieser Funktion
     muss bei mehreren Interruptquellen das Interrupt-
     request Register INTRQ ausgewertet werden, hier
     Timer2
   -------------------------------------------------------- */
void interrupt(void) __interrupt(0)
{
  static uint8_t cx= 0;

  if (INTRQ & INTRQ_TM2)
  {
    // LED blinken geschieht ausschliesslich im Interrupthandler

    cx++;
    cx= cx % 31;
    // 31 * 32.768 = 1.015 s
    if (!cx)  led_toggle();

    INTRQ &= ~INTRQ_TM2;          // Interruptanforderung quittieren
  }
}

/* --------------------------------------------------
                           main
   -------------------------------------------------- */
void main(void)
{
  led_init();
  tim2_init();

  while(1);
}
