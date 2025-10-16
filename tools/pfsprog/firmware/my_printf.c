/* --------------------------- my_printf.c  ----------------------------

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

#include "my_printf.h"

char printfkomma = 1;


/* ------------------------------------------------------------
                            PUTINT
     gibt einen Integer dezimal aus. Ist Uebergabe
     "komma" != 0 wird ein "Kommapunkt" mit ausgegeben.

     Bsp.: 12345 wird als 123.45 ausgegeben.
     (ermoeglicht Pseudofloatausgaben im Bereich)
   ------------------------------------------------------------ */
void putint(int16_t i, char komma)
{
  typedef enum boolean { FALSE, TRUE }bool_t;

  static uint16_t zz[]      = { 10000, 1000, 100, 10 };
  bool_t          not_first = FALSE;

  uint8_t    zi;
  int16_t    z, b;

  komma= 5-komma;

  if (!i)
  {
    my_putchar('0');
  }
  else
  {
    if(i < 0)
    {
      my_putchar('-');
      i = -i;
    }

    for(zi = 0; zi < 4; zi++)
    {
      z = 0;
      b = 0;

      if  ((zi== komma) && komma)
      {
        if (!not_first) my_putchar('0');
        my_putchar('.');
        not_first= TRUE;
      }

      while(z + zz[zi] <= i)
      {
        b++;
        z += zz[zi];
      }
      if(b || not_first)
      {
        my_putchar('0' + b);
        not_first = TRUE;
      }
      i -= z;
    }
    if (komma== 4) my_putchar('.');
    my_putchar('0' + i);
  }
}



/* --------------------------------------------------
                    HEXNIBBLEOUT
     gibt die unteren 4 Bits eines chars als Hexa-
     ziffer aus. Eine Pruefung ob die oberen vier
     Bits geloescht sind erfolgt NICHT !
  --------------------------------------------------  */
void hexnibbleout(uint8_t b)
{
  if (b< 10) b+= '0'; else b+= 55;
  my_putchar(b);
}

/* --------------------------------------------------
                       PUTHEX
      gibt einen Integer hexadezimal aus. Ist die
      auszugebende Zahl >= 0xff erfolgt die Ausgabe
      2-stellig, ist sie groesser erfolgt die
      Ausgabe 4-stellig.
   -------------------------------------------------- */
void puthex(uint16_t h)
{
  uint8_t b;

  if (h> 0xff)                    // 16 Bit-Wert
  {
    b= (h >> 12);
    hexnibbleout(b);
    b= (h >> 8) & 0x0f;
    hexnibbleout(b);
  }
  b= h;
  b= (h >> 4) & 0x0f;
  hexnibbleout(b);
  b= h & 0x0f;
  hexnibbleout(b);
}

/* --------------------------------------------------
                  MY_PUTRAMSTRING
     gibt einen String aus dem RAM aus
   -------------------------------------------------- */
void my_putramstring(uint8_t *p)
{
  do
  {
    my_putchar( *p );
  } while( *p++);
}


/* --------------------------------------------------
                       OWN_PRINTF

                      als Macros:
                       MY_PRINTF
                      TINY_PRINTF

     alternativer Ersatz fuer printf.

     Aufruf:

         own_printf(PSTR("Ergebnis= %d"),zahl);

     oder durch define-Makro (besser):

         tiny_printf("Ergebnis= %d",zahl);
         my_printf("Ergebnis= %d",zahl);

     Platzhalterfunktionen:

        %s     : Ausgabe Textstring
        %d     : dezimale Ausgabe
        %x     : hexadezimale Ausgabe
                 ist Wert > 0xff erfolgt 4-stellige
                 Ausgabe
                 is Wert <= 0xff erfolgt 2-stellige
                 Ausgabe
        %k     : Integerausgabe als Pseudokommazahl
                 12345 wird als 123.45 ausgegeben
        %c     : Ausgabe als Asciizeichen

   -------------------------------------------------- */
void own_printf(const uint8_t *s,...)
{
  int       arg1;
  uint8_t   *arg2;
  char      ch;
  va_list   ap;

  va_start(ap,(const)s);
  do
  {
    ch= pgm_read_byte(s);
    if(ch== 0) return;

    if(ch=='%')            // Platzhalterzeichen
    {
      uint8_t token= pgm_read_byte(++s);
      switch(token)
      {
        case 'd':          // dezimale Ausgabe
        {
          arg1= va_arg(ap,int);
          putint(arg1,0);
          break;
        }
        case 'x':          // hexadezimale Ausgabe
        {
          arg1= va_arg(ap,int);
          puthex(arg1);
          break;
        }
        case 'k':          // Integerausgabe mit Komma: 12896 zeigt 128.96 an
        {
          arg1= va_arg(ap,int);
          putint(arg1, printfkomma);
          break;
        }
        case 'c':          // Zeichenausgabe
        {
          arg1= va_arg(ap,int);
          my_putchar(arg1);
          break;
        }
        case '%':
        {
          my_putchar(token);
          break;
        }
        case 's':
        {
          arg2= va_arg(ap,char *);
          my_putramstring(arg2);
          break;
        }
      }
    }
    else
    {
      my_putchar(ch);
    }
    s++;
  }while (ch != '\0');
}
