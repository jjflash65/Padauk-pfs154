/* -------------------------------------------------------
                         ukw_radio.c

     Firmware fuer Sendersuchlaufradio mit RDA5807M
     ( I2C Interface ).

     Compiler  : SDCC 4.0.3
     MCU       : PFS154 / PFS173

     14.10.2020        R. Seelig

   ------------------------------------------------------ */

/* ------------------------------------------------------

   Pinbelegung RDA5807 Modul

                                 RDA5807-Modul
                                --------------
                           SDA  |  1  +-+ 10  |  Antenne
                           SCL  |  2  +-+  9  |  GND
                           GND  |  3       8  |  NF rechts
                            NC  |  4 _____ 7  |  NF links
                         +3.3V  |  5 _____ 6  |  GND
                                 -------------

   ------------------------------------------------------ */


#include <stdint.h>
#include "pdk_init.h"
#include "pfs1xx_gpio.h"
#include "delay.h"

#include "i2c.h"
#include "rda5807.h"


// LED-Anschlusspin
#define led_init()          PA0_output_init()
#define led_set()           PA0_clr()
#define led_clr()           PA0_set()
#define led_toggle()        PA0_toggle()

#define blinken_an()        __engint()
#define blinken_aus()       __disgint()

// Leuchtbandanzeige auf PortB, gemeinsame Anode an +Vcc
#define ledband_init()      PBC= 0xff
#define ledband_set(value)  PB= value

// Tastenanschlusspins
#define keyup_init()        PA4_input_init()
#define is_keyup()          (!(is_PA4()))

#define keydwn_init()       PA3_input_init()
#define is_keydwn()         (!(is_PA3()))

#define keysel_init()       PA7_input_init()
#define is_keysel()         (!(is_PA7()))

enum {scan_mode, volume_mode};


/* --------------------------------------------------
      volume_show

      zeigt die aktuell eingestellte Lautstaerke
      auf der LED-Bandanzeige an
   -------------------------------------------------- */
void volume_show(void)
{
  uint8_t vol;
  char i,s;

  vol= 0x80;
  s= aktvol >> 1;
  for (i= 0; i< s; i++)
  {
    vol= vol >> 1;
    vol |= 0x80;
  }
  ledband_set(~vol);
}

/* --------------------------------------------------
      freq_show

      zeigt die aktuell eingestellte Empfangs-
      frequenz des Empfaengers auf der
      LED-Bandanzeige an
   -------------------------------------------------- */
void freq_show(void)
{
  if (aktfreq< 900) { ledband_set(0x7f); return; }
  if (aktfreq< 930) { ledband_set(0xbf); return; }
  if (aktfreq< 950) { ledband_set(0xdf); return; }
  if (aktfreq< 980) { ledband_set(0xef); return; }
  if (aktfreq< 1000) { ledband_set(0xf7); return; }
  if (aktfreq< 1020) { ledband_set(0xfb); return; }
  if (aktfreq< 1050) { ledband_set(0xfd); return; }
  ledband_set(0xfe);
  return;
}

/* --------------------------------------------------------
                            t16_init

     initialisiert den 16-Bit Timer mit Interruptaus-
     loesung fuer automatisches Blinken

   -------------------------------------------------------- */
void t16_init(void)
{
  T16M = (uint8_t)(T16M_CLK_IHRC | T16M_CLK_DIV64 | T16M_INTSRC_15BIT);

  __engint();                   // grundsaetzlich Interrupt zulassen
  INTEN |= INTEN_T16;           // Timerinterrupt zulassen
}

/* --------------------------------------------------------
                           interrupt

     der Interrupt-Handler, laesst LED blinken und zeigt
     die aktuell eingestellte Frequenz auf der
     LED-Bandanzeige an
   -------------------------------------------------------- */
void interrupt(void) __interrupt(0)
{

  if (INTRQ & INTRQ_T16)
  {
    led_toggle();
    freq_show();

    INTRQ &= ~INTRQ_T16;          // Interruptanforderung quittieren
  }
}

/* --------------------------------------------------
                           main
   -------------------------------------------------- */
void main(void)
{
  uint8_t selmode = scan_mode;      // Flag zur Auswahl, ob mittels keyup / keydwn der
                                    // Sendersuchlauf gestartet wird oder die Lautstaerke
                                    // eingestellt werden soll
  MISCLVR= (uint8_t)MISCLVR_1V8;
  delay(1500);                      // warten bis die Spannungen an den Peripheriebausteinen
                                    // sicher ansteht.
  led_init();
  t16_init();
  blinken_aus();
  ledband_init();
  led_clr();
  i2c_master_init();

  ledband_set(0);

  delay(300);
  rda5807_reset();
  delay(100);
  rda5807_poweron();
  delay(100);
  rda5807_setmono();
  aktfreq= 1018;
  rda5807_setfreq();
  aktvol= 3;
  rda5807_setvol();

  freq_show();

  while(1)
  {

    keysel_init();
    delay(2);
    if (is_keysel())
    {

      ledband_set(0xaa);

      if (selmode== scan_mode)
      {
        selmode= volume_mode;
        led_set();                // auf LED anzeigen, dass Volumemodus aktiv ist
        volume_show();
      }
      else
      {
        selmode= scan_mode;
        led_clr();                // abgeschaltete LED zeigt Suchlaufmodus an
        freq_show();

      }
      delay(20);
      while(is_keysel());
      delay(20);

    }

    // Suchlaufmodus
    if (selmode== scan_mode)
    {
      keyup_init();
      delay(2);
      if (is_keyup())            // Sendersuchlauf starten, zunehmende Frequenz
      {
        blinken_an();            // blinkende LED zeigt Suchlauf an
        delay(20);
        while(is_keyup());       // warten bis Taste losgelassen wird
        delay(20);
        i2c_master_init();
        rda5807_scanup();
        freq_show();
        blinken_aus();
        led_clr();               // Suchlauf mit ausgeschalteter LED verlassen
      }

      keydwn_init();
      delay(2);
      if (is_keydwn())           // Sendersuchlauf starten, abnehmende Frequenz
      {
        blinken_an();            // blinkende LED zeigt Suchlauf an
        delay(20);
        while(is_keydwn());       // warten bis Taste losgelassen wird
        delay(20);
        i2c_master_init();
        rda5807_scandown();
        freq_show();
        blinken_aus();
        led_clr();               // Suchlauf mit ausgeschalteter LED verlassen
      }
    }
    else
    // Volumemodus
    {

      keyup_init();
      delay(2);
      if (is_keyup())            // Lautstaerke erhoehen
      {
        delay(20);
        while(is_keyup());       // warten bis Taste losgelassen wird
        delay(20);
        if (aktvol < 15) aktvol++;
        rda5807_setvol();
        volume_show();
      }

      keydwn_init();
      delay(2);
      if (is_keydwn())           // Lautstaerke vermindern
      {
        delay(20);
        while(is_keydwn());      // warten bis Taste losgelassen wird
        delay(20);
        if (aktvol) aktvol--;
        rda5807_setvol();
        volume_show();
      }

    }
  }
}

