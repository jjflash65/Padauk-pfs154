/* -------------------------------------------------------
                      ir_receiver_switch.c

     IR-Empfaenger (38 kHz) schaltet 8 Kanaele ueber
     Schieberegister, Realisierung unter Verwendung
     Timer2

     Hardware : HX1838 IR-Empfaenger
              : 7-Segmentanzeige mit TM1637 Controller
              : Schieberegister SN74HC595 / HEF4094

     MCU      : PFS154

     Pinbelegung :

       Dout HX1839 ---- PB0

       CLK - TM1637 --- PA7
       DIO - TM1637 --- PA6

       DATA - SR    --- PB1
       STROBE - SR  --- PB2
       CLK - SR     --- PB3


     17.12.2020  R. Seelig
   ------------------------------------------------------ */

/*
      +5 V                                    +5 V
        ^                                       ^
        |                                       |
        |                                       |
        |                                       |
        |                                       |
        |                                       |
        |                                       |
        |                    +-------------+    |
        |                    |   HX1838    |    |
        |                    |             |    |
   -----------+              | OUT GND +5V |    |
              |              +-------------+    |
              |                 |   |   |       |
              |                 |   |   |       |
     PFS154   |                 |   |   +-------+
              |                 |   |
              |                 |   |
          PB0 |-----------------+   |
              |                     |
              |                     |
              |                     |
              |                     |
              |                     |
   -----------+                    ---
        |
        |
        |
       ---
*/

#include <stdint.h>
#include "pdk_init.h"
#include "pfs1xx_gpio.h"
#include "delay.h"
#include "tm1637_seg7.h"


#include "hx1838.h"

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



/* --------------------------------------------------------
                       interrupt

     der Interrupt-Handler. Innerhalb dieser Funktion
     muss bei mehreren Interruptquellen das Interrupt-
     request Register INTRQ ausgewertet werden.

     Hier verwendet als externer Interrupt 0 auf PA0
   -------------------------------------------------------- */
void interrupt(void) __interrupt(0)
{

  // externer Interupt 0 auf PA0
  if (INTRQ & INTRQ_PA0)
  {

    ir_receive();

    INTRQ &= ~INTRQ_PA0;
  }

}


/* ------------------------------------------------------------------------------
                                     M A I N
    ----------------------------------------------------------------------------- */
void main(void)
{
  ir_init();

  hellig= 11;
  tm1637_init();
  sr_init();

  sr_setvalue(sr_value);
  tm1637_sethex(0);

  while(1)
  {

    if (ir_newflag)
    {
      tm1637_sethex(ir_code);

      // schaltet die Kanaele, erster Programmlauf  um die Codes zu ermitteln,
      // dann hier die Codes in den Case-Faellen modifizieren
      // Bitmasken zum Togglen der Kanaele sind in pfs1xx_gpio.h deklariert
      switch (ir_code)
      {
        case (0x0df2) : sr_value ^= MASK0; sr_setvalue(sr_value); break;  // Kanal-0
        case (0x0cf3) : sr_value ^= MASK1; sr_setvalue(sr_value); break;  // Kanal-1
        case (0x18e7) : sr_value ^= MASK2; sr_setvalue(sr_value); break;  // Kanal-2
        case (0x5ea1) : sr_value ^= MASK3; sr_setvalue(sr_value); break;  // Kanal-3
        case (0x08f7) : sr_value ^= MASK4; sr_setvalue(sr_value); break;  // Kanal-4
        case (0x1ce3) : sr_value ^= MASK5; sr_setvalue(sr_value); break;  // Kanal-5
        case (0x5aa5) : sr_value ^= MASK6; sr_setvalue(sr_value); break;  // Kanal-6
        case (0x42bd) : sr_value ^= MASK7; sr_setvalue(sr_value); break;  // Kanal-7

        default : break;
      }

      ir_newflag= 0;    // neu eingetroffenes Datum quittieren
    }
  }
}
