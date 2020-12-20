/* ---------------------------------------------------------
                     pdk14_init.h

     Initialisierung des Systemtakts des Controllers, darf
     nur von der Datei, die main enthaelt eingebunden
     werden, da hier ausnahmsweise Code und Variable
     definiert sind.

     Compiler  : SDCC 4.0.3
     MCU       : PFS154 / PFS173

     14.10.2020        R. Seelig

   --------------------------------------------------------- */

#ifndef in_PFS_init
  #define in_PFS_init

  #include <pdk/device.h>
  #include <easy-pdk/calibrate.h>


//  #define FACTORYCAL     0

/* ---------------------------------------------------------
     leider sind die Register PADIER / PBDIER nur
     write-only und deshalb funktioniert ein read - modify -
     write back nicht. Werden Anschluesse jedoch im
     Programmablauf zwischen Ein- und Ausgang umgeschaltet
     werden hierdurch bereits als Eingaenge initialisierte
     Pins in ihrer Konfiguration geloescht. Deshalb werden
     hier - ausnahmsweise - in einem Header globale
     Variable definiert
   --------------------------------------------------------- */

  uint8_t PADIER_TMP= 0x00;
  uint8_t PBDIER_TMP= 0x00;

/* ---------------------------------------------------------
                  _sdcc_external_startup

     Initialisierung des Systemtaktes. Ausnahmsweise MIT
     Codesequenz im Headerfile, weil diese Funktion nicht
     manuell gestartet wird sondern automatisch mit ein-
     gebunden wird.

     Hinweis: Bei der Compilierung des Programms muss ein
     #define F_CPU vorhanden sein, weil nach dem Wert fuer
     F_CPU der Systemtakt des Controllers eingestellt wird
   --------------------------------------------------------- */
  unsigned char _sdcc_external_startup(void)
  {

    #if (FACTORYCAL == 1)

      IHRCR = GET_FACTORY_IHRCR();

      #if (F_CPU == 16000000)
        MISCLVR = MISCLVR_4V;
        CLKMD = 0x54;
      #elif (F_CPU == 8000000)
        CLKMD = 0x34;
      #elif (F_CPU == 4000000)
        CLKMD = 0x14;
      #elif (F_CPU == 2000000)
        CLKMD = 0x3c;
      #elif (F_CPU == 1000000)
        CLKMD = 0x1c;
      #elif (F_CPU == 500000)
        CLKMD = 0x7c;
      #else
        #error "valid values for F_CPU are: 16, 8, 4, 2, 1, 0.5  MHz"
      #endif


    #else

      #if (F_CPU > 12000000)
        CLKMD = CLKMD_IHRC | CLKMD_ENABLE_IHRC;               // 16 Mhz main clock
      #elif (F_CPU > 6000000)
        CLKMD = CLKMD_IHRC_DIV2 | CLKMD_ENABLE_IHRC;          // 8 Mhz main clock
      #elif (F_CPU > 3000000)
        CLKMD = CLKMD_IHRC_DIV4 | CLKMD_ENABLE_IHRC;          // 4 Mhz main clock
      #elif (F_CPU > 1500000)
        CLKMD = CLKMD_IHRC_DIV8 | CLKMD_ENABLE_IHRC;          // 2 Mhz main clock
      #elif (F_CPU > 750000)
        CLKMD = CLKMD_IHRC_DIV16 | CLKMD_ENABLE_IHRC;         // 1 Mhz main clock
      #elif (F_CPU > 375000)
        CLKMD = CLKMD_IHRC_DIV32 | CLKMD_ENABLE_IHRC;         // 0.5 Mhz main clock
      #elif (F_CPU > 182000)
        CLKMD = CLKMD_IHRC_DIV64 | CLKMD_ENABLE_IHRC;         // 0.25 Mhz main clock
      #else
        #error "valid values for F_CPU are: 16, 8, 4, 2, 1, 0.5, 0.25 MHz"
      #endif

      // PDK_USE_FACTORY_TRIMMING();                          // It appears that the factory trimming is sometimes slightly off - possibly not done at 5V
      EASY_PDK_CALIBRATE_IHRC(F_CPU,5000);
    #endif

    return 0;
  }


#endif
