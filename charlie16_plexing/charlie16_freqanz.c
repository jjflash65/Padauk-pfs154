/* -------------------------------------------------------
                        charlie_freqanz.c

     Beispiel fuer die Anzeige eines eingestellten
     UKW-Senders auf einer Leuchtanzeige mit 20 LED's

     Compiler  : SDCC 4.0.3
     MCU       : PFS154 / PFS173

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
                          show_freq

     20 LED's als Frequenzanzeige eines UKW-Empfaengers
     Uebergabe:
        freq : ist der anzuzeigende Frequenzwert [100 kHz]
               950 entspricht somit 95 MHz anzuzeigen
   ------------------------------------------------------ */
void show_freq(uint16_t freq)
{
  uint16_t v1;
  uint16_t  a;

  if (freq> 1070) freq= 1070;
  if (freq< 880) freq= 880;

  v1= (freq / 10)-88;                          // Freqquenzband auf Bereich von 0..19 reduziert
  a= ((uint16_t) 15 * (uint16_t) v1 ) / 19;    // Zahlenbereich reduziert auf 0..16;
  if (a> 15) a== 15;
  charlie16_buf= (uint16_t)1 << a;

}



/* ------------------------------------------------------
                            main
   ------------------------------------------------------ */
int main(void)
{
  uint16_t i;

  charlie16_init();

  show_freq(1070);              // zeige 880 MHz auf der Leuchtbandanzeige an
  delay(1000);

  // simuliere einen Sendersuchlauf von 88.0 bis 107.0 MHz in
  // 0.1 MHz Schritten

  for (i= 880; i< 1071; i++)
  {
    show_freq(i);
    delay(50);
  }
  while(1);
}
