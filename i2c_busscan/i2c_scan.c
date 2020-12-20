/*--------------------------------------------------------
                             i2c_scan.c

     I2C-Busscanner mittels Software-I2C (Bitbanging)

     Scant den I2C Bus nach angeschlossenen Teilnehmern und gibt
     die Hexadresse des I2C-Device auf dem UART aus.

     Hinweis: das R-/W -Bit wird als Bestandteil der Adresse ange-
              sehen. Adressen werden also als 8-Bit Adresse
              verstanden). Somit ist dann ein Device 2 mal im Adress-
              raum vorhanden: Geradezahlige Adressen sind somit die
              "Leseadressen", ungeradzahlige die "Schreibadressen"


     Compiler  : SDCC 4.0.3
     MCU       : PFS154 / PFS173

     07.10.2020        R. Seelig

  -------------------------------------------------------- */

#include <stdint.h>
#include "pdk_init.h"
#include "pfs1xx_gpio.h"
#include "delay.h"

#include "my_printf.h"
#include "uart.h"
#include "i2c.h"

// LED-Anschlusspin
#define led_init()     PA0_output_init()
#define led_set()      PA0_clr()
#define led_clr()      PA0_set()


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

/* --------------------------------------------------
                           main
   -------------------------------------------------- */
void main(void)
{
  char ch;
  uint8_t cx,anz;
  uint8_t ack;

  led_init();
  led_clr();
  i2c_master_init();
  delay(10);
  uart_init();
  delay(10);

  while(1)
  {

    printf("\n\rI2C Bus scanning\n\r--------------------------\n\n\r" \
           "Devices found at address:\n\n\r");

    anz= 0;

    led_set();
    for (cx= 0; cx< 254; cx += 2)
    {
      ack= i2c_start(cx);
      delay(1);
      i2c_stop();
      if (ack)
      {
        switch (cx)
        {
          case 0xC0 : printf("Adr. %xh : TEA5767 UKW-Radio\n\r", cx); break;
          case 0x20 :
          case 0x22 : printf("Adr. %xh : RDA5807 UKW-Radio\n\r", cx); break;
          case 0x40 :
          case 0x42 :
          case 0x44 :
          case 0x46 :
          case 0x48 :
          case 0x4A :
          case 0x4C :
          case 0x4E : printf("Adr. %xh : PCF8574 I/O Expander\n\r", cx); break;
          case 0x90 :
          case 0x92 :
          case 0x94 :
          case 0x96 :
          case 0x98 :
          case 0x9A :
          case 0x9C :
          case 0x9E : printf("Adr. %xh : LM75 Temp.-Sensor\n\r", cx); break;
          case 0xA0 :
          case 0xA2 :
          case 0xA4 :
          case 0xA6 :
          case 0xA8 :
          case 0xAA :
          case 0xAC :
          case 0xAE : printf("Adr. %xh : EEProm\n\r", cx); break;
          case 0x78 : printf("Adr. %xh : SSD13016 I2C-OLED Display\n\r", cx); break;
          case 0xD0 : printf("Adr. %xh : RTC - DS1307\n\r", cx); break;

          default   : printf("Adr. %xh : unknown\n\r",cx); break;
        }
      }
      delay(1);
    }
    led_clr();


    printf("\n\n\rEnd of I2C-bus scanning... \n\n\r");
    i2c_stop();

    uart_init();
    printf("Press any key for rescan... \n\r");
    delay(30);
    ch= uart_getchar();
    printf("\n\n\r\n\n\r");
  }
}
