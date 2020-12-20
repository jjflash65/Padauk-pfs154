/*--------------------------------------------------------
                         adc_pfs154.c

     Softwaremodul zur Realisierung eines ADC mithilfe
     des internen Komparators und einer sehr einfachen
     externen Konstantstromquelle.

     Aufloesung des so generierten ADC : 8-Bit

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

#include "adc_pfs154.h"

extern uint8_t PADIER_TMP;
extern uint8_t PBDIER_TMP;

volatile static uint16_t ticks;


/* --------------------------------------------------------
                            adc_init

     initialisiert Anschluesse des Komparators als
     analoge Eingaenge und den 8 Bit Timer3 fuer die
     spaeteren Zaehlung der Timerticks
   -------------------------------------------------------- */
void adc_init(void)
{
  PAC &= ~(MASK3 | MASK4);               // PA3 und PA4 als Eingang
  PAPH &= ~(MASK3 | MASK4);              // und keine Pullup-Widerstaende
  PADIER_TMP &= ~(MASK3 | MASK4);        // PA3 und PA4 Digitaleingang disable
  PADIER= PADIER_TMP;                    // Register schreiben

  ticks= 0;

  // Timer3 initialisieren
  TM3C = (uint8_t)TM3C_CLK_IHRC;
  TM3S = (uint8_t)TM3S_PRESCALE_NONE | TM2S_SCALE_NONE;
  TM3B = 0x80;

  __engint();                           // Interrupts grundsaetzlich zulassen
  discharge();
}

/* --------------------------------------------------------
                        adc_count_ticks

     zaehlt die Interruptintervalle waehrend der
     Spannungsmessung und setzt einen Reload-Wert fuer
     den 16-Bit Timer. Muss in den Interrupthandler
     eingefuegt werden
   -------------------------------------------------------- */
void adc_count_ticks(void)
{
  if (ticks< 255) ticks++;       // grundsaetzlich ist es
                                 // denkbar, eine hoehere
                                 // Aufloesung als 8 Bit zu erreichen
}

/* --------------------------------------------------------
                        adc_getvalue

     ermittelt die an PA4 angelegte Spannung, 8 Bit
     Aufloesung
   -------------------------------------------------------- */
uint8_t adc_getvalue(void)
{
  GPCC= 0x00;                       // Komparator aus und PA3 somit als
                                    // digitalen Ausgang zum Kondensator entladen
                                    // verwenden
  discharge_off();                  // Kondensator entladen abschalten

  PAC &= ~MASK3;                    // PA3 wieder auf Eingang
  PAPH &= ~MASK3;                   // und keine Pullup-Widerstaende
  PADIER_TMP &= ~MASK3;             // PA3 disable digital input = analoger Eingang enable
  PADIER= PADIER_TMP;               // und entsprechendes Register schreiben
  GPCC= comp_config;                // Komparator einschalten
  ticks= 0;
  INTEN |= INTEN_TM3;               // Timerinterrupt zum "ticks" zaehlen zulassen
  while(!(GPCC & 0x40));            // Bit6 (Mask 0x40) ist das Ergebnisbit des Komparators
  INTEN &= ~(INTEN_TM3);            // Timerinterrupt sperren, damit die Delay-Zeiten
                                    // u.a. fuer die serielle Schnittstelle wieder stimmen
  discharge();                      // Kondensator entladen

  delay(2);
  return ticks;
}
