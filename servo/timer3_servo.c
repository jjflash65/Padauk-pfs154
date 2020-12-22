/*--------------------------------------------------------
                       timer3_servo.c

     Ansteuerung eines Servomotors unter Verwendung
     Timer3

     Compiler  : SDCC 4.0.3
     MCU       : PFS154

     17.12.2020        R. Seelig

  -------------------------------------------------------- */

#include <stdint.h>
#include "pdk_init.h"
#include "pfs1xx_gpio.h"

#include "delay.h"

#define servo_init()    PA0_output_init()
#define servo_set()     PA0_set()
#define servo_clr()     PA0_clr()
#define servo_toggle()  PA0_toggle()

uint16_t servo_winkel = 75;

/* --------------------------------------------------------
                          tim3_init

     Beteiligte Register: TM3C (timer3 control register)
                          TM3C (timer3 counter register)
                          TM3S (timer3 scalar reigster)

     siehe timer_2_3_reg.txt

     erzeugt einen Interrupt alle 10 us
   -------------------------------------------------------- */
void tim3_init(void)
{
  TM3C = 0x10;
  TM3S = 0x01;
  TM3B = 69;


  __engint();                   // grundsaetzlich Interrupt zulassen
  INTEN |= INTEN_TM3;           // Timerinterrupt zulassen
  TM3CT= 0x00;                  // Counter auf 0 setzen
}

/* --------------------------------------------------------
                       interrupt

     der Interrupt-Handler. Innerhalb dieser Funktion
     muss bei mehreren Interruptquellen das Interrupt-
     request Register INTRQ ausgewertet werden, hier
     Timer3
   -------------------------------------------------------- */
void interrupt(void) __interrupt(0)
{
  static uint16_t serv_cx= 0;

  if (INTRQ & INTRQ_TM3)
  {
    // Servofrequenz ist 50Hz und entspricht somit einer Periodendauer
    // von 20 ms
    // Pulsdauer von  1 ms entspricht linkem Anschlag, Pulsdauer von
    // 1.5 ms entspricht Mittelstellung, 2.0 ms rechter Anschlag
    // Hierher wird durch Timereinstellung alle 20 us verzweigt, d.h.
    // es gibt bei einer Pulsdauerdifferenz von 1 ms :
    //          1000 us / 20 us = 50 anfahrbare Stellungen
    // globale Variable servo_winkel bestimmt die Auslenkung des
    // Servomotors. 50 == linker Anschlag
    //              100 == rechter Anschlag

    TM3CT= 0;
    serv_cx++;
    if (serv_cx== servo_winkel) servo_clr();
    serv_cx= serv_cx % 1000;       // 1000 * 20 us = 20 ms
    if (!serv_cx) servo_set();     // zu Periodenbeginn mit Pulsdauer beginnen

    INTRQ &= ~INTRQ_TM3;           // Interruptanforderung quittieren
  }
}

/* --------------------------------------------------
                           main
   -------------------------------------------------- */
void main(void)
{
  uint8_t i;

  servo_init();
  tim3_init();
  servo_winkel= 50;

  // das verwendete Servo des Demos hier benoetigte andere
  // Puls-Pause Verhaeltnis und so wird der Auschlag links-
  // rechts mit Werten von 40 .. 50 realisier !
  while(1)
  {
    for (i= 40; i< 150; i++)
    {
      servo_winkel= i;
      delay(2);
    }
    for (i= 150; i> 40; i--)
    {
      servo_winkel= i;
      delay(2);
    }
  }
}
