/*--------------------------------------------------------
                              blink_int.c

     Blinkprogramm mittels interruptgesteuertem Timer
     realisiert.

     Der 16-Bit Timer generiert jede Millisekunde eine
     Interruptanforderung

     Compiler  : SDCC 4.0.3
     MCU       : PFS154 / PFS173

     07.10.2020        R. Seelig

  -------------------------------------------------------- */

#include <stdint.h>
#include "pdk_init.h"
#include "pfs1xx_gpio.h"

// LED-Anschlusspin
#define led_init()     PA0_output_init()
#define led_set()      PA0_set()
#define led_clr()      PA0_clr()
#define led_toggle()   PA0_toggle()

volatile uint16_t ticks;                       // wird durch den Interrupthandler fortgezaehlt
volatile uint16_t millis;                      // Zaehlervariable fuer hochgezaehlte Millisekunden

volatile uint16_t reload = 489;                // hier sollte 382 angegeben werden, jeder andere
                                               // Wert sind Abweichungen von Laufzeit und Toleranz

/* --------------------------------------------------------
                            t16_init

     initialisiert den 16-Bit Timer mit Interruptaus-
     loesung.

     Beschreibung siehe t16.txt

   -------------------------------------------------------- */
void t16_init(void)
{

  T16M = (uint8_t)(T16M_CLK_IHRC | T16M_CLK_DIV1 | T16M_INTSRC_14BIT);

  // Reloadwert des 16-Bit Timers setzen
  __asm

    mov a,_reload+0
    mov __t16c+0,a
    mov a,_reload+1
    mov __t16c+1,a

    stt16 __t16c
  __endasm;

  ticks = 0;
  millis = 0;
  __engint();                   // grundsaetzlich Interrupt zulassen
  INTEN |= INTEN_T16;           // Timerinterrupt zulassen
}

/* --------------------------------------------------------
                       interrupt

     der Interrupt-Handler

     Leider hat sich (scheinbar) ein BUG in SDCC 4.0.3
     bzgl. des Schreibens eines 16-Bit Timerwertes ein-
     geschlichen und deshalb wird das Zaehlerregister
     des Timers mittels Maschinenbefehle beschrieben

   -------------------------------------------------------- */
void interrupt(void) __interrupt(0)
{

  if (INTRQ & INTRQ_T16)
  {
    millis++;
    ticks= 0;

    // Reloadwert des 16-Bit Timers setzen
    __asm

      mov a,_reload+0
      mov __t16c+0,a
      mov a,_reload+1
      mov __t16c+1,a

      stt16 __t16c
    __endasm;

    INTRQ &= ~INTRQ_T16;          // Interruptanforderung quittieren
  }
}

/* --------------------------------------------------
                           main
   -------------------------------------------------- */
void main(void)
{
  char led_stat = 0;

  led_init();
  t16_init();

  while(1)
  {
    while (millis < 500);
    led_toggle();
    millis= 0;
  }
}
