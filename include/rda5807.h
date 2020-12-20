/* -----------------------------------------------------
                          rda5807.h

    Header fuer grundsaetzliche Steuerungsfunktionen
    zum UKW-Receiver RDA5807 mit I2C Schnittstelle

      Compiler  : SDCC 4.0.3
      MCU       : PFS154 / PFS173

    21.10.2020   R. Seelig
  ------------------------------------------------------ */

#ifndef in_rda
  #define in_rda

  #include <stdint.h>
  #include "i2c.h"

  #define  rda5807_adrs    0x20           // I2C-addr. fuer sequientielllen Zugriff
  #define  rda5807_adrr    0x22           // I2C-addr. fuer wahlfreien Zugriff
  #define  rda5807_adrt    0xc0           // I2C-addr. fuer TEA5767 kompatiblen Modus

  #define fbandmin         870            // 87.0  MHz unteres Frequenzende
  #define fbandmax         1080           // 108.0 MHz oberes Frequenzende
  #define sigschwelle      72             // Schwelle ab der ein Sender als "gut empfangen" gilt

  extern uint16_t aktfreq;                // Startfrequenz ( 101.8 MHz fuer neue Welle)
  extern uint8_t  aktvol;                 // Startlautstaerke


  void rda5807_writereg(void);
  void rda5807_write(void);
  void rda5807_reset(void);
  void rda5807_poweron(void);
  int rda5807_setfreq();
  void rda5807_setvol();
  void rda5807_setmono(void);
  void rda5807_setstereo(void);
  uint8_t rda5807_getsig(void);
  void rda5807_scandown(void);
  void rda5807_scanup(void);

#endif
