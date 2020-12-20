/*--------------------------------------------------------
                        pwm.c

     Verwendung eines 8-Bit Timer als PWM, hier Timer2.

     Timer3 hat  andere moegliche Ausgangspins, an-
     sonsten sind die Timer identisch, sieh
     timer2_3_reg.txt

     Compiler  : SDCC 4.0.3
     MCU       : PFS154

     17.12.2020        R. Seelig

  -------------------------------------------------------- */

#include <stdint.h>
#include "pdk_init.h"
#include "pfs1xx_gpio.h"

// PWM Ausgangsanschluesse
#define pwmout_init()   PB2_output_init()

// der Spannungspegel [mv], den ein Hi-Level besitzt
#define own_ub     4920


/* --------------------------------------------------------
                          tim2_init

     Beteiligte Register: TM2C (timer2 control register)
                          TM2C (timer2 counter register)
                          TM2S (timer2 scalar reigster)
   -------------------------------------------------------- */
void tim2_init(void)
{
  // TM2C
  // 0010  01             10                         = 0x26
  // IHRC  output PB2     pwm-mode / no invers
  TM2C = 0x26;
  // -----------------------------------------------

  // TM2S
  // 0       00         0001            = 0x01
  // PWM-8   Prescale   divisior
  // f_pwmc = IHRC / 256 = 16MHz / 256 = 62.5 kHz
  // f_pwm  = f_pwm / Prescale / (divisor+1) = 31.25 / 1 / 2 =15.625
  // f_timer= sysclock / 64 / 5 = 25 kHz
  TM2S = 0x00;
  // -----------------------------------------------

  // TM2B
  // 160 = 0xa0 Teile Hi, 255-160 Teile LO
  // 128 = 0x80 Teile Hi, 255-128 Teile LO
  TM2B = 0x80;

}

/* --------------------------------------------------
                      voltage_set

     setzt ein PWM-Signal, dass einen RMS Wert von
     Value entspricht. Um aus diesem PWM-Signal mit
     der angegebenen Spannung eine reale analoge
     Spannung erzeugt werden, muss dieses PWM-Signal
     mindestens mit einem RC-Tiefpassfilter gefiltert
     werden.

     Uebergabe:
       value : Spannung in Milivolt
   -------------------------------------------------- */
void voltage_set(uint32_t value)
{
  uint32_t r;

  r= (value * 255) / own_ub;
  TM2B= (uint8_t)r;
}


/* --------------------------------------------------
                           main
   -------------------------------------------------- */
void main(void)
{
  pwmout_init();
  tim2_init();
  voltage_set(3220);

  while(1);
}
