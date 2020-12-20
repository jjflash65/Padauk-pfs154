/*--------------------------------------------------------
                           uart_demo.c

    Demoprogramm fuer serielle Schnittstelle und Ausgabe
    mittels abgespecktem printf.

     Compiler  : SDCC 4.0.3
     MCU       : PFS154 / PFS173

     03.11.2020        R. Seelig

    ToDo: Delay-Zeiten fuer PFS173 muessen noch ausge-
    messen werden

  -------------------------------------------------------- */

#include <stdint.h>
#include "pdk_init.h"
#include "pfs1xx_gpio.h"

#include "uart.h"
#include "my_printf.h"


#define printf         my_printf


/* --------------------------------------------------------
   my_putchar

   wird von my-printf / printf aufgerufen und hier muss
   eine Zeichenausgabefunktion angegeben sein, auf das
   printf dann schreibt !
   -------------------------------------------------------- */
void my_putchar(char ch)
{
  uart_putchar(ch);
}

/* --------------------------------------------------------
                           main
   -------------------------------------------------------- */
void main(void)
{
  uint16_t counter = 0;
  char ch;

  uart_init();

  // Endlosschleife
  while(1)
  {
    uart_puts("Taste: ");
    ch= uart_getchar();
    printf("\n\r   ==> Taste war: %c\n\r", ch);

    printf("Counter: %d\n\r", counter);
    counter++;
  }
}
