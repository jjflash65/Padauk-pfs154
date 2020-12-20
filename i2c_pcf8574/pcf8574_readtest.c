/*--------------------------------------------------------
                           pcf8574_readtest.c

     Test des pcf8754 als Eingabeerweiterung

     Compiler  : SDCC 4.0.3
     MCU       : PFS154 / PFS173

     14.10.2020        R. Seelig

  -------------------------------------------------------- */

#include <stdint.h>
#include "pdk_init.h"
#include "pfs1xx_gpio.h"
#include "delay.h"

#include "i2c.h"
#include "uart.h"
#include "my_printf.h"


#define printf         my_printf


// LED-Anschlusspin
#define led_init()     PA0_output_init()
#define led_set()      PA0_clr()
#define led_clr()      PA0_set()


// I2C-Adresse des I/O Expanders
#define pcf8574_addr   0x40

/* #################################################################
     Funktionen PCF8574 I/O-Expander
   ################################################################# */

/* --------------------------------------------------
                       pcf8574_write

     sendet ein Byte an den I/O-Expander
   -------------------------------------------------- */
void pcf8574_write(uint8_t value)
{
  i2c_start(pcf8574_addr);
  i2c_write(value);
  i2c_stop();
}

/* --------------------------------------------------
                       pcf8574_write

     liest ein Byte vom I/O-Expander
   -------------------------------------------------- */
uint8_t pcf8574_read(void)
{
  uint8_t b;

  pcf8574_write(0xff);                            // alle Ausgaenge des Expanders
                                                  // auf logisch 1
  i2c_start(pcf8574_addr | 1);                   // zum lesen
  b= i2c_read_nack();
  i2c_stop();
  return b;
}

void my_putchar(uint8_t ch)
{
  uart_putchar(ch);
}


/* --------------------------------------------------
                           main
   -------------------------------------------------- */
void main(void)
{
  uint8_t b,i;
  uint16_t cx;

  led_init();
  i2c_master_init();
  uart_init();
  printf("\n\r PCF8574 Lesetest\n\n\r");

  delay(1);
/*
  i2c_master_init();
  delay(1);
  i= i2c_start(pcf8574_addr);

  if (i) led_set();           // "Onboard LED" zeigt an, wenn der I/O Expander
                              // gefunden wurde
  i2c_stop();
*/
  cx= 0;
  while(1)
  {
    pcf8574_write(0xff);      // Ausgaenge auf 1 damit eine angelegte 0
                              // als solche gelesen werden kann
    b= (pcf8574_read());
    printf("\r Count: %d; Pins: 0x%x ",cx,b);
    delay(1000);
    cx++;
  }
}
