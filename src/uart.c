/* ------------------------------------------------------
                         uart.h

     Softwaremodul einer Bitbanging erzeugten seriellen
     Schnittstelle

     Compiler  : SDCC 4.0.3
     MCU       : PFS154 / PFS173

     06.10.2020        R. Seelig

  ------------------------------------------------------- */

#include <stdint.h>
#include <stdlib.h>

#include "uart.h"

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
                   uart_init
    serielle Schnittstelle initialisieren.

    In uart.h sind folgende Wertezuweisungen zu
    taetigen fuer:

    uart_txbit, uart_rxbit  : Anschluesse der UART
    uart_delfak             : Konstantenwert bestimmt
                              die Baudrate

    Protokoll:  8N1
   -------------------------------------------------- */
void uart_init(void)
{
  uarttx_init();
  uartrx_init();
}

/* --------------------------------------------------
                     uart_putchar
    Zeichen ueber die serielle Schnittstelle senden
   -------------------------------------------------- */
void uart_putchar(uint8_t ch)
{
  uarttx_clr();                 // Startbit
  uart_delay(uart_delfak);
  for (char i= 0; i< 8; i++)
  {
    if (ch & 0x01) uarttx_set(); else uarttx_clr();
    uart_delay(uart_delfak);
    ch >>= 1;
  }
  uarttx_set();
  uart_delay(uart_delfak);
}

#if (uart_puts_enable == 1)
  /* --------------------------------------------------
                      uart_puts
        String ueber serielle Schnittstelle senden
     -------------------------------------------------- */
  void uart_puts(char *p)
  {
    do
    {
      uart_putchar( *p );
    } while( *p++);
  }
#endif

/* --------------------------------------------------
                    uart_getchar
      Zeichen von serieller Schnittstelle lesen
   -------------------------------------------------- */
char uart_getchar()
{
  char ch= 0;
  uint8_t mask= 1;

  while (is_uartrx());                  // auf Startbit warten
  uart_delay(uart_delfak+(uart_delfak/2));
  for (char i= 0; i< 8; i++)
  {
    if (is_uartrx()) ch |= mask; else ch &= ~(mask);
    uart_delay(uart_delfak);
    mask <<= 1;
  }
  return ch;
}
