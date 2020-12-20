/*--------------------------------------------------------
                         adc_demo_02.c

     Einfaches "Panelmeter" fuer 2-stellige Spannungs-
     anzeige auf einer gemultiplexten 7-Segmentanzeige

     Zusaetzliche Ausgabe auf serieller Schnittstelle.

     Eingangsspannungsbereich: 0 - 9.9V

     Aufloesung des so generierten ADC : 7-Bit

     Timer3 8-Bit wird verwendet.

     Compiler  : SDCC 4.0.3
     MCU       : PFS154 / PFS173

     02.11.2020        R. Seelig
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
     exakt 9.90 V an den Eingangsteiler anschliessen
     und den Trimmer so einstellen, dass die Anzeige
     gerade so von 9.8V nach 9.9V umspringt.

                + 5V      + 5V
                  ^         ^
                  |         |
                  |         |
                  |        +-+
                  |        | |
                  |        | | 4,7k
                  |        | |
               / +-+       +-+
              / \| |        |                        1 M
                 \ | 50k    |                        ____
                 |\        |<             Umess o---|____|---+-----> PA4
                 | |\------|                                 |
                 +-+       |\                                |
                  |         |                               +-+
                  |         |                               | |
   PA3 <--------------------+                               | | 100k
                  |         |                               | |
                  |         |  470n                         +-+
                  |        ---                               |
                  |        ---                               |
                  |         |                                |
                 ---       ---                              ---

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
#include "seg7mpx_dig2.h"

uint8_t mpx_enable = 1;

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

  uart_init();
  adc_init();
  seg7_init();
  segmode= zmode;
  seg7_dp= 1;

  printf("\n\n\r Spannungsmessung mit Padauk PFS154");
  printf("\n\r --------------------------------------\n\n\r");

  cx= 0;
  while(1)
  {
    mpx_enable= 0;                      // Multiplex fuer Dauer der ADC aus.
    adc_value= adc_getvalue() >> 1;     // Messbereich auf 7 Bit reduzieren um Spannung mit
                                        // einer Nachkommastelle besser darstellen zu koennen
    mpx_enable= 1;                      // MPX wieder an

    delay(5);
    printfkomma= 1;
    printf("\n\r%d: %kV   ", cx, adc_value);
    s7buf= hex2bcd(adc_value) & 0xff;

    delay(500);
    cx++;

  }
}
