/*--------------------------------------------------------
                           lm75_demo2.c

    Auslesen des LM75 - I2C Sensors und Ausgabe der
    Temperatur auf 3-stelliger 7-Segmentanzeige (mit
    gemeinsamer Kathode)

    Compiler  : SDCC 4.0.3
    MCU       : PFS154 / PFS173

    07.10.2020        R. Seelig

  -------------------------------------------------------- */

#include <stdint.h>
#include "pdk_init.h"
#include "pfs1xx_gpio.h"
#include "delay.h"

#include "i2c.h"
#include "seg7mpx_dig3.h"


#define lm75_addr      0x90


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
                       interrupt

     der Interrupt-Handler. TM2 ruft den Interrupt mit
     ca. 1 kHz auf.

     Hier muss das Multiplexen der Anzeige eingehaengt
     werden.
   -------------------------------------------------------- */
void interrupt(void) __interrupt(0)
{
  // Interruptquelle ist Timer2
  if (INTRQ & INTRQ_TM2)
  {
    seg7_mpx();                   // Timergesteuertes multiplexen
                                  // der 2-stelligen Anzeige
    INTRQ &= ~INTRQ_TM2;          // Interruptanforderung quittieren
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

  seg7_init();
  i2c_master_init();
  seg7_dp= 1;

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
        s7buf= 0xfff;
        delay(250);
        s7buf= 0x000;
        delay(250);
      }
    }while (!ack);

    __disgint();           // damit 16-Bit Operationen nicht durch das
                          // Multiplexen unterbrochen werden

    temp = lm75_read();
    s7buf= hex2bcd16(temp);

    __engint();            // Interrupts wieder an

    delay(500);

  }
}
