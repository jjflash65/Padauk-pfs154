/* ----------------------------------------------------------
                        shiftreg_demo.c

     Demoprogramm zum Ansteuern eines seriellen Schiebe-
     registers (HEF4094 oder SN74HC595).

     Funktionen sind zu klein, als dass hieraus eine .c / .h
     Kombination sinnvoll waere:

     ==> Funktionen mittels copy / paste in andere Projekte
     einfuegen


     MCU       :  PFS154 / PFS173

     Anmerkung : Programm sollte mit allen AVR Controllern
                 laufen, da nur Bitbanging verwendet wird

     18.12.2020  R. Seelig
   ---------------------------------------------------------- */
/*
                          PFS154 / PFS173
                           Anschlusspins

                         +----------------+
                     PB4 | 1   |___|   16 | PB3
                     PB5 | 2           15 | PB2
                     PB6 | 3           14 | PB1
                     PB7 | 4  PFS154   13 | PB0
                         -------------
                     VDD | 5 (1)   (8) 12 | GND
                     PA7 | 6 (2)   (7) 11 | PA0
             ICPDA / PA6 | 7 (3)   (6) 10 | PA4
               Vpp / PA5 | 8 (4)   (5)  9 | PA3 / ICPCK
                         +----------------+                 7


             74HC595                           HEF4094
         Anschlusspins IC                  Anschlusspins IC

          +------------+                    +------------+
          |            |                    |            |
     out1 | 1   H   16 | +Vcc           str | 1   H   16 | +Vcc
     out2 | 2   E   15 | out0           dat | 2   E   15 | oe
     out3 | 3   F   14 | dat            clk | 3   F   14 | out4
     out4 | 4       13 | /oe           out0 | 4       13 | out5
     out5 | 5   4   12 | strobe        out1 | 5   4   12 | out6
     out6 | 6   0   11 | clk           out2 | 6   0   11 | out7
     out7 | 7   9   10 | /rst          out3 | 7   9   10 | out_seriell'
      gnd | 8   4    9 | out7s         gnd  | 8   4    9 | out_seriell
          |            |                    |            |
          +------------+                    +------------+


               Anschlussbelegung

    PFS154             HEF4094               SN74HC595
   ------------------------------------------------------------
                       16 Vcc                16 Vcc
                       15 oe = Vcc           13 (/oe) = GND
   14 (PB1)    ...      2 (seriell data)     14 (serial data)3
   15 (PB2)    ...      1 (strobe)           12 (strobe)
   16 (PB3)    ...      3 (clock)            11 (clock)
                       10 (seriell out')     10 (/res) = Vcc
                        8 GND                 9 (serial out; Q7s)
                                              8 GND
*/
#include <stdint.h>
#include "pdk_init.h"
#include "pfs1xx_gpio.h"
#include "delay.h"


// ----------------------------------------------------------------
//   Anbindung Schieberegister <==> Microcontroller
// ----------------------------------------------------------------
#define srdat_init()      PB1_output_init()
#define srdat_set()       PB1_set()
#define srdat_clr()       PB1_clr()

#define srclk_init()      PB3_output_init()
#define srclk_set()       PB3_set()
#define srclk_clr()       PB3_clr()

#define srstrobe_init()   PB2_output_init()
#define srstrobe_set()    PB2_set()
#define srstrobe_clr()    PB2_clr()



// initialisert alle 3 beteiligten Pins als Ausgaenge
#define sr_init()         { srdat_init(); srclk_init(); srstrobe_init(); }


uint8_t sr_value = 0x00;                // Puffervariable des Schieberegisters
                                        // da dieses nicht gelesen werden kann

/* ----------------------------------------------------------
                             sr_setvalue

     transferiert einen 8-Bit Wert ins serielle Schiebe-
     register
   ---------------------------------------------------------- */
void sr_setvalue(uint8_t value)
{
  int8_t i;

  for (i= 7; i> -1; i--)
  {
    if ((value >> i) &1)  srdat_set(); else srdat_clr();

    srclk_set();
    srclk_clr();                            // Taktimpuls erzeugen
  }

  srstrobe_set();                           // Strobeimpuls : Daten Schieberegister ins Ausgangslatch uebernehmen
  srstrobe_clr();
  sr_value= value;
}

/* ----------------------------------------------------------
                             sr_setsinglebit

     setzt ein einzelnes Ausgabnsbit im Schieberegister indem
     die Puffervariable "bitmanipuliert" wird und die Puffer-
     variable ausgegeben wird
   ---------------------------------------------------------- */
void sr_setsinglebit(char nr)
{
  sr_value |= (1 << nr);
  sr_setvalue(sr_value);
}

/* ----------------------------------------------------------
                             sr_clrsinglebit

     loescht ein einzelnes Ausgabnsbit im Schieberegister indem
     die Puffervariable "bitmanipuliert" wird und die Puffer-
     variable ausgegeben wird
   ---------------------------------------------------------- */
void sr_clrsinglebit(char nr)
{
  sr_value &= ~(1 << nr);
  sr_setvalue(sr_value);
}

#define sr_clear()   sr_setvalue(0x00)

#define ledspeed     130

/* ----------------------------------------------------------
                               MAIN
   ---------------------------------------------------------- */
int main(void)
{
  int8_t i;

  sr_init();

  while(1)
  {
    // 8 LED aufblenden
    for (i= 0; i< 8; i++)
    {
      sr_setsinglebit(i);
      _delay_ms(ledspeed);
    }
    // 8 LED abblenden
    for (i= 7; i> -1; i--)
    {
      sr_clrsinglebit(i);
      _delay_ms(ledspeed);
    }

    // Wechselblinker
    for (i= 0; i< 10; i++)
    {
      sr_setvalue(0xaa);
      _delay_ms(ledspeed);
      sr_setvalue(0x55);
      _delay_ms(ledspeed);
    }
    sr_clear();
  }
}
