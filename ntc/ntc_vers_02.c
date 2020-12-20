/*--------------------------------------------------------
                          ntc_vers_01.c

     Thermometer mit Padauk PFS1xx und NTC 10k.

     Ausgabe der Temperatur auf:

       - serielle Schnittstelle
       - 2 stellige gemultiplexte 7-Segmentanzeige

     Zusaetzlich benoetigte Hardware:
       - 2 stellige Anzeige, gemeinsame Kathode
       - einfache Konstantstromquelle aus Widerstand,
         Transistor und Kondensator
       - NTC 10k und Vorwiderstand 10k

       Compiler  : SDCC 4.0.3
       MCU       : PFS154 / PFS173

    03.11.2020        R. Seelig

  -------------------------------------------------------- */

#include <stdint.h>
#include "pdk_init.h"
#include "pfs1xx_gpio.h"
#include "delay.h"

#include "adc_pfs154.h"
#include "seg7mpx_dig2.h"
#include "uart.h"

uint8_t mpx_enable = 1;


#define ledminus_init()    PA0_output_init()
#define ledminus_set()     PA0_clr()
#define ledminus_clr()     PA0_set();

#define crlf()      { uart_putchar(13); uart_putchar(10); }

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
    uart_putchar('0');
  }
  else
  {
    if(i < 0)
    {
      uart_putchar('-');
      i = -i;
    }
    for(zi = 0; zi < 4; zi++)
    {
      z = 0;
      b = 0;

      if  ((zi==komma) && komma)
      {
        if (!not_first) uart_putchar('0');
        uart_putchar('.');
        not_first= TRUE;
      }

      while(z + zz[zi] <= i)
      {
        b++;
        z += zz[zi];
      }
      if(b || not_first)
      {
        uart_putchar('0' + b);
        not_first = TRUE;
      }
      i -= z;
    }
    if (komma== 4) uart_putchar('.');
    uart_putchar('0' + i);
  }
}

/* -------------------------------------------------
     Lookup-table fuer NTC-Widerstand
     R25-Wert: 10.00 kOhm
     Pullup-Widerstand: 10.00 kOhm
     Materialkonstante beta: 3950
     Aufloesung des ADC: 8 Bit
     Einheit eines Tabellenwertes: 0.1 Grad Celcius
     Temperaturfehler der Tabelle: 0.7 Grad Celcius
   -------------------------------------------------*/
const int ntctable[] = {
  1269, 1016, 763, 621, 520, 439, 370, 308,
  250, 194, 139, 83, 22, -47, -132, -256,
  -380
};

/* -------------------------------------------------
                     ntc_gettemp

    zuordnen des Temperaturwertes aus gegebenem
    ADC-Wert.
   ------------------------------------------------- */
int16_t ntc_gettemp(uint8_t adc_value)
{
  int16_t p1,p2;

  // Stuetzpunkt vor und nach dem ADC Wert ermitteln.
  p1 = ntctable[ (adc_value >> 4)    ];
  p2 = ntctable[ (adc_value >> 4) + 1];

  // zwischen beiden Punkten interpolieren.
  return p1 - ( (p1-p2) * (adc_value & 0x000f) ) / 16;
}


/* --------------------------------------------------------
                       interrupt

     der Interrupt-Handler. Hier muss ueber die ver-
     schiedenen Interruptquellen gepollt werden
   -------------------------------------------------------- */
void interrupt(void) __interrupt(0)
{

  // Interruptquelle Timer3,
  // Zaehler des realisierten Analog-Digitalwandlers
  if (INTRQ & INTRQ_TM3)
  {

    adc_count_ticks();

    INTRQ &= ~INTRQ_TM3;          // Interruptanforderung quittieren
  }

  // Interruptquelle Timer2
  // Multiplexen der 7-Segmentanzeige
  if (INTRQ & INTRQ_TM2)
  {
    if (mpx_enable)
      seg7_mpx();                 // Timergesteuertes multiplexen
                                  // der 2-stelligen Anzeige
    INTRQ &= ~INTRQ_TM2;          // Interruptanforderung quittieren
  }
}


/* --------------------------------------------------------
                           main
   -------------------------------------------------------- */

void main(void)
{
  int16_t temp;
  uint16_t cx;
  int8_t s7temp;
  uint8_t i;

  volatile uint16_t adc_value;

  adc_init();
  uart_init();
  seg7_init();
  ledminus_init();

  uart_puts("\n\n\r");

  cx= 0;
  while(1)
  {
    mpx_enable= 0;                      // Multiplex fuer Dauer der ADC aus.
    adc_value= adc_getvalue();
    temp= ntc_gettemp(adc_value);
    if (temp< 0)
    {
      s7temp= -(temp / 10);
      ledminus_set();
    }
    else
    {
      s7temp = temp / 10;
      ledminus_clr();
    }
    s7buf= hex2bcd(s7temp);
    mpx_enable= 1;                      // MPX wieder an

    putint(cx,0);
    uart_puts(" # ");
    putint(temp,1);
    uart_puts("\n\r");

    delay(500);
    cx++;
  }
}
