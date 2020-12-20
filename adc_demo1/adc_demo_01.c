/*--------------------------------------------------------
                         adc_demo_01.c

     Demo fuer die Realisierung eines ADC mithilfe des
     internen eingebauten Komparators und einer sehr
     einfachen externen Konstantstromquelle.

     Ausgabe des gemessenen analogen Wertes ueber
     serielle Schnittstelle.

     Aufloesung des so generierten ADC : 8-Bit

     Timer3 8-Bit wird verwendet.

     Compiler  : SDCC 4.0.3
     MCU       : PFS154 / PFS173

     29.10.2020        R. Seelig
  -------------------------------------------------------- */

/* -------------------------------------------------------
     Funktionsprinzip:

     Eine sehr einfache Konstantstromquelle laedt einen
     Kondensator der dadurch einen (nahezu) linearen
     Spannungsanstieg verzeichnet. In Ermangelung der
     Moeglichkeit des Abspeicherns von Kalibrierdaten
     wird der Konstantstrom mittels Trimmer eingestellt.

     PA3 schliesst den Kondensator kurz und dieser ist
     somit leer. Wird PA3 nun von digitalem Ausgang auf
     analogen Eingang umgeschaltet ist PA3 nun der
     + Eingang des Komparators. Die Zeit wird gezaehlt,
     bis die Spannung am Kondensator die Spannung am
     - Eingang des Komparators ueberschreitet, die ver-
     gangene Zeit entspricht der Hoehe der angelegten
     Spannung.

     Um den ADC zu kalibrieren, eine Spannung von
     exakt 2.40 V anschliessen und den Trimmer so
     einstellen, dass die Ausgabe gerade so von
     2.39 nach 2.40 umspringt.

                + 5V      + 5V
                  ^         ^
                  |         |
                  |         |
                  |        +-+
                  |        | |
                  |        | | 4,7k
                  |        | |
               / +-+       +-+
              / \| |        |
                 \ | 50k    |
                 |\        |<                 Umess o---+------> PA4
                 | |\------|                            |
                 +-+       |\                           |
                  |         |                          +-+
                  |         |                          | |
   PA3 <--------------------+                          | | 1M
                  |         |                          | |
                  |         |  470n                    +-+
                  |        ---                          |
                  |        ---                          |
                  |         |                           |
                 ---       ---                         ---

    PA3 : Spannung am Kondensator;
    PA4 : zu messende Spannung
  -------------------------------------------------------- */

#include <stdint.h>

#include "pdk_init.h"
#include "pfs1xx_gpio.h"
#include "delay.h"
#include "uart.h"                             // u.a. Anschlusspins fuer RxD / TxD
#include "my_printf.h"
#include "adc_pfs154.h"

#define led_init()        PA0_output_init();
#define led_on()          PA0_clr();          // LED ist aktiv Low
#define led_off()         PA0_set();
#define led_toggle()      PA0_toggle();

/* --------------------------------------------------------
                       interrupt

     der Interrupt-Handler. Hier muss ueber die ver-
     schiedenen Interruptquellen gepollt werden
   -------------------------------------------------------- */
void interrupt(void) __interrupt(0)
{

  if (INTRQ & INTRQ_TM3)
  {

    // hier wird der Zaehler des realisierten Analog-Digitalwandlers
    // eingehaengt. Zaehlt die Ticks, die vergehen, bis die Spannung
    // an PA4 groesser ist, als die Spannung an PA3
    adc_count_ticks();

    INTRQ &= ~INTRQ_TM3;          // Interruptanforderung quittieren
  }
}


/* --------------------------------------------------------
   my_putchar

   wird von my-printf / printf aufgerufen und hier muss
   eine Zeichenausgabefunktion angegeben sein, auf das
   printf dann schreibt !
   -------------------------------------------------------- */
void my_putchar(uint8_t ch)
{
  uart_putchar(ch);
}

/* --------------------------------------------------------
                              main
   -------------------------------------------------------- */
void main(void)
{
  volatile uint16_t adc_value;

  uint16_t cx;

  led_init();
  uart_init();
  adc_init();

  led_on();

  printf("\n\n\r Spannungsmessung mit Padauk PFS154");
  printf("\n\r --------------------------------------\n\n\r");

  cx= 0;
  while(1)
  {
    adc_value= adc_getvalue();
    delay(5);
    printfkomma= 2;
    printf("\r Messw.-Nr.: %d; ADC: %kV   ", cx, adc_value);
    delay(500);
    cx++;

    led_toggle();
  }
}
