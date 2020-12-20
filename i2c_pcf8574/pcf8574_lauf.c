/*--------------------------------------------------------
                           pcf8574_lauf.c

     Lauflicht am I2C-8574 Portextender als Demo

     Compiler  : SDCC 4.0.3
     MCU       : PFS154 / PFS173

     14.10.2020        R. Seelig

  -------------------------------------------------------- */

#include <stdint.h>
#include "pdk_init.h"
#include "pfs1xx_gpio.h"
#include "delay.h"

#include "i2c.h"

// LED-Anschlusspin
#define led_init()     PA0_output_init()
#define led_set()      PA0_clr()
#define led_clr()      PA0_set()


// I2C-Adresse des I/O Expanders
#define pcf8574_addr   0x40

#define laufspeed      80


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

  pcf8574_write(0xff);                          // alle Ausgaenge des Expanders
                                                // auf logisch 1
  i2c_start(pcf8574_addr | 1);                  // zum lesen
  b= i2c_read_nack();
  i2c_stop();
  return b;
}


/* --------------------------------------------------
                           main
   -------------------------------------------------- */
void main(void)
{
  uint8_t i;
  uint8_t lauf;

  led_init();
  delay(1);
  i2c_master_init();
  delay(1);
  i= i2c_start(pcf8574_addr);

  if (i) led_set();           // "Onboard LED" zeigt an, wenn der I/O Expander
                              // gefunden wurde

  lauf= 0x01;
  pcf8574_write(~lauf);
  delay(laufspeed);
  while(1)
  {
    for (i= 0; i<7; i++)
    {
      lauf = lauf << 1;
      pcf8574_write(~lauf);
      delay(laufspeed);
    }
    for (i= 0; i<7; i++)
    {
      lauf = lauf >> 1;
      pcf8574_write(~lauf);
      delay(laufspeed);
    }
  }
}
