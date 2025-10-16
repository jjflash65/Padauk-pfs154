/*--------------------------------------------------------
                           lm75_demo.c

    Auslesen des LM75 - I2C Sensors und Ausgabe der
    Temperatur auf N5110 Display und serieller
    Schnittstelle

    Compiler  : SDCC 4.0.3
    MCU       : PFS154 / PFS173

    07.10.2020        R. Seelig

  -------------------------------------------------------- */

#include <stdint.h>
#include "pdk_init.h"
#include "pfs1xx_gpio.h"
#include "delay.h"

#include "uart.h"
#include "i2c.h"
#include "n5110.h"

// LED-Anschlusspin
#define led_init()     PA0_output_init()
#define led_set()      PA0_clr()
#define led_clr()      PA0_set()


#define lm75_addr      0x90

enum { lcdout, uartout };

uint8_t io_channel = uartout;


/* #################################################################
     Funktionen des LM75 Temperatursensors
   ################################################################# */

/* --------------------------------------------------
     lm75_read

     liest den Temperatursensor aus.

     Rueckgabewert:
        gelesene Temperatur x 10.

        Bsp.: Rueckgabewert von 245 entspricht 24.5 Grad Celsius
   -------------------------------------------------- */
int lm75_read(void)
{
  char       ack;
  char       t1;
  uint8_t    t2;
  int        lm75temp;

  ack= i2c_start(lm75_addr);                // LM75 Basisadresse
  if (ack)
  {

    i2c_write(0x00);                        // LM75 Registerselect: Temp. auslesen
    i2c_write(0x00);

    i2c_stop();

    ack= i2c_start(lm75_addr | 1);          // LM75 zum Lesen anwaehlen
    t1= 0;
    t1= i2c_read_ack();                     // hoeherwertigen 8 Bit
    t2= i2c_read_nack();                    // niederwertiges Bit (repraesentiert 0.5 Grad)
    i2c_stop();

  }
  else
  {
    i2c_stop();
    return -127;                            // Abbruch, Chip nicht gefunden
  }

  lm75temp= t1;
  lm75temp = lm75temp*10;
  if (t2 & 0x80) lm75temp += 5;             // wenn niederwertiges Bit gesetzt, sind das 0.5 Grad
  return lm75temp;
}

/* -----------------------------------------------------------------
     Ende LM75
   ----------------------------------------------------------------- */

/* --------------------------------------------------------
                          my_putchar
   -------------------------------------------------------- */
void my_putchar(char ch)
{
  if (io_channel== lcdout)
  {
    lcd_putchar(ch);
  }
  else
  {
    uart_putchar(ch);
  }
}


/* ------------------------------------------------------------
                            putint
     gibt einen Integer dezimal aus. Ist Uebergabe
     "komma" != 0 wird ein "Kommapunkt" mit ausgegeben.

     Bsp.: 12345 wird als 123.45 ausgegeben.
     (ermoeglicht Pseudofloatausgaben im Bereich)
   ------------------------------------------------------------ */
void putint(int i, char komma)
{
  typedef enum boolean { FALSE, TRUE }bool_t;

  static int zz[]      = { 10000, 1000, 100, 10 };
  bool_t     not_first = FALSE;

  char       zi;
  int        z, b;

  komma= 5-komma;

  if (!i)
  {
    my_putchar('0');
  }
  else
  {
    if(i < 0)
    {
      my_putchar('-');
      i = -i;
    }
    for(zi = 0; zi < 4; zi++)
    {
      z = 0;
      b = 0;

      if  ((zi==komma) && komma)
      {
        if (!not_first) my_putchar('0');
        my_putchar('.');
        not_first= TRUE;
      }

      while(z + zz[zi] <= i)
      {
        b++;
        z += zz[zi];
      }
      if(b || not_first)
      {
        my_putchar('0' + b);
        not_first = TRUE;
      }
      i -= z;
    }
    if (komma== 4) my_putchar('.');
    my_putchar('0' + i);
  }
}

/* --------------------------------------------------
                       prints
      gibt einen durch *p adressierten String ueber
      my_putchar aus.
   -------------------------------------------------- */
void prints(char *c)
{
  while (*c)
  {
    my_putchar(*c++);
  }
}


/* --------------------------------------------------
                           main
   -------------------------------------------------- */
void main(void)
{
  uint8_t cx;
  uint8_t ack;
  uint8_t temp;

  led_init();
  led_clr();
  i2c_master_init();

  led_set();
  delay(10);
  uart_init();
  lcd_init();
  delay(10);
  clrscr();

  io_channel= uartout;
  prints("\n\r LM75 - I2C Temperatursensor \n\n\r");
  io_channel= lcdout;
  gotoxy(0,0);
  prints("LM75-I2C Temp.");
  gotoxy(0,1);
  prints("--------------");
  gotoxy(0,4);
  prints("Temp: 00.0 |C");        // das Zeichen "|" entspricht in der Font-Tabelle
                                  // einem hochgestellten kleinen "o"

  led_set();
  cx= 0;
  while(1)
  {
    // Abfragen, ob ein LM75 vorhanden ist, so lange die LED
    // ein- und ausschalten, BIS ein LM75 vorhanden ist
    do
    {
      ack= i2c_start(lm75_addr);          // Abfrage, ob LM75 gefunden wird
      i2c_stop();
      if (!ack)
      {
        led_clr();
        delay(250);
        led_set();
        delay(250);
      }
    }while (!ack);

    temp= lm75_read();

    // Ausgabe auf Uart
    io_channel= uartout;
    prints("\r Nr.: ");
    putint(cx,0);
    prints(" # Temp: ");
    putint(temp,1);             // Kommastelle einfuegen

    // Ausgabe auf LCD
    io_channel= lcdout;
    gotoxy(6,4);
    putint(temp,1);             // Kommastelle einfuegen
    cx++;
    delay(500);

  }
}
