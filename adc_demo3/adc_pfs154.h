/*--------------------------------------------------------
                         adc_pfs154.h

     Header fuer Softwaremodul zur Realisierung eines ADC
     mithilfe des internen Komparators und einer sehr
     einfachen externen Konstantstromquelle.

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

#ifndef in_adcpfs154
  #define in_adcpfs154

  #include <stdint.h>
  #include <pdk/device.h>
  #include "pfs1xx_gpio.h"
  #include "delay.h"

  #define discharge()       { PA3_output_init(); PA3_clr(); }
  #define discharge_off()   PA3_input_init()

  // Konfigurationsbyte Comparator Control Register fuer:
  // Comparator enable, +Ue = PA4, -Ue = PA3, invertierter Ausgang
  #define comp_config       0x91;

  extern uint8_t PADIER_TMP;
  extern uint8_t PBDIER_TMP;

/* -------------------------------------------------------
                          Prototypen
   ------------------------------------------------------- */

  void adc_count_ticks(void);
  uint8_t adc_getvalue(void);
  void adc_init(void);


#endif
