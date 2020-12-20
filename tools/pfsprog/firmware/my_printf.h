/* --------------------------- my_printf.h  ----------------------------

     Implementierung einer sehr minimalistischen (und im Funktionsumfang
     eingeschraenkten) Version von printf zur urspruenglichen Verwendung
     im ATtiny2313 (sollte allerdings auch mit allen anderen AVR-
     Controllern funktionieren)

     Benoetigt "irgendwo" im Gesamtprogram ein

                my_putchar(char c);


     22.04.2016   R. Seelig

     Danke an www.mikrocontroller.net (Dr.Sommer, beric)

     letzte Aenderung:
     ---------------------------------------------------

     Anpassung an die my-printf Versionen von STM8 und STM32 Versionen
     mit Angabe der Nachkommastellen bei Verwendung des %k Platzhalters
     (Implementation der globalen Variable printfkomma und veraenderte
     < putint > Funktion

     29.08.2018    R. Seelig

   --------------------------------------------------------------------- */

#ifndef in_myprintf
  #define in_myprintf

  #include <avr/io.h>
  #include <avr/pgmspace.h>
  #include <stdarg.h>

  extern char printfkomma;

  extern void my_putchar(char c);
  void putint(int16_t i, char komma);
  void hexnibbleout(uint8_t b);
  void puthex(uint16_t h);
  void my_putramstring(uint8_t *p);

  void own_printf(const uint8_t *s,...);

  #define tiny_printf(str,...)  (own_printf(PSTR(str), ## __VA_ARGS__))
  #define my_printf             tiny_printf


#endif


