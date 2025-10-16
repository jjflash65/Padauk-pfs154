/* -------------------------------------------------------
                        charlie_uart.c

     Demoprogramm fuer Ansteuerung von 16 LED's mittels
     Charlieplexing. Der anzuzeigende 16-Bit Wert wird
     ueber den UART mit 2400 Bd empfangen.

     Da das Plexing der Anzeige mittels Timerinterrupt
     das Timing der seriellen Schnittstelle veraendert,
     muss fuer die Datenuebertragung der Interrupt-
     handler ausgeschaltet werden, was zu einem kurzen
     Flackern der Anzeige fuehrt.

     Zwischen dem 1. und 2. Byte des Senders muss
     mindestens eine Wartezeit von 40 ms  eingehalten
     werden.

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

/*
              Delay-Werte Tabelle fuer unterschiedliche Baudraten

    F_CPU   | 2400 Bd | 4800 Bd | 9600 Bd | 19200 Bd | 38400 Bd |  57600 Bd | 115200 Bd |
    -------------------------------------------------------------------------------------
       8    |   182   |   88    |   44    |    22    |    10    |     6     |
      16    |         |  182    |   88    |    44    |    22    |    14     |     6
*/

#define uart_delfak       182               // 2400 Bd / 8 MHz

#define uartrx_init()     PA4_input_init()
#define is_uartrx()        is_PA4()

/* --------------------------------------------------
                       uart_delay
      Warteschleife zum Einstellen der Baudraten
   -------------------------------------------------- */
void uart_delay(uint16_t cnt)
{
  volatile int i;              // Volatile, damit der Compiler die Warteschleife
                               // nicht "wegoptimiert"

  for (i= 0; i< cnt; i++);
}


/* --------------------------------------------------
                    uart_getchar
      Zeichen von serieller Schnittstelle lesen
   -------------------------------------------------- */
uint8_t uart_getchar()
{
  char ch= 0;
  uint8_t mask= 1;

  while (is_uartrx());                  // auf Startbit warten
  __disgint();                          // Interrupts aus
  uart_delay(uart_delfak+(uart_delfak/2));
  for (char i= 0; i< 8; i++)
  {
    if (is_uartrx()) ch |= mask; else ch &= ~(mask);
    uart_delay(uart_delfak);
    mask <<= 1;
  }
  return ch;
}


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

    INTRQ &= ~INTRQ_T16;                    // Interruptanforderung quittieren
  }
}

/* ------------------------------------------------------
                      charlie16_getword

     liest 2 Byte von der seriellen Schnittstelle.
     Ist der zeitliche Sendeabstand zwischen 1. und 2.
     Byte > 200 ms ist ein Timeout aufgetreten und die
     Funktion wird mit Rueckgabewert 0 verlassen.

     Sind 2 Byte eingetroffen wird der Wert 1 zurueck
     gegeben
   ------------------------------------------------------ */
uint8_t charlie16_getword(uint16_t *value)
{
  uint16_t  b1, b2;
  uint32_t  timout;

  timout= 0;
  b1= uart_getchar();

  while (is_uartrx() && (timout < 1000000))    // auf Startbit warten
  {
    timout++;
  }
  if (timout> 999999)
  {
  __engint();
    return 0;                             // das zweite Zeichen ist nicht angekommen
  }
  b2= uart_getchar();
  *value= ((b1 << 8) & 0xff00) | ( b2 & 0x00ff);
  __engint();
  return 1;
}


/* ------------------------------------------------------
                            main
   ------------------------------------------------------ */
int main(void)
{
  uint16_t w;
  uint8_t noerr;

  charlie16_init();
  charlie16_buf= 0xaaaa;
  uartrx_init();

  while(1)
  {
    noerr= charlie16_getword(&w);
    if (noerr)
      charlie16_buf= w;
    delay(5);
  }
}

