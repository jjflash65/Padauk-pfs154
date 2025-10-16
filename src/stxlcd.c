/* ---------------------------------------------------------
                            stxlcd.c

     Softwaremodul zum Ansprechen eines ueber einen
     Padauk PFS154 Controller gesteuerten Textdisplays
     mittels einer single-wire Verbindung

     Datenleitung: PA4 (in stxlcd.h aenderbar)

     MCU      : STM8Sx003 / 105
     Compiler : SDCC 4.01 oder neuer

     zusaetzliche Hardware:
        PFS154 - Textdisplay

     19.10.2024   R. Seelig
   --------------------------------------------------------- */

#include "stxlcd.h"

uint8_t wherex, wherey;

/* --------------------------------------------------
                     stxt_sendval

     sendet einen Wert an ein an einen PFS154 ange-
     schlossenes Textdisplay

     Uebergabe:
        val  : Integervariable, die Steuer- und
               datenbits enthalten. Es werden
               die niederwertigen 12 Bits seriell auf
               dem Datenpin gesendet.

               Protokoll siehe in stxlcd.recv.c
   -------------------------------------------------- */
void stxt_sendval(uint16_t val)
{
  uint8_t  i;
  uint16_t mask;

  stx_clr();
  delay_us(200);

  mask= 0x800;
  for (i= 0; i< 12; i++)
  {

    if (val & mask)
    {
      stx_set();
      delay_us(pulselength);
      stx_clr();
      delay_us(pulsepause);
    }
    else
    {
      stx_set();
      delay_us(pulsepause);
      stx_clr();
      delay_us(pulselength);
    }
    mask= mask >> 1;
  }
  stx_set();
  delay(3);
}


/* --------------------------------------------------
                 stxt_senduserchar

     sendet ein Userzeichen bestehend aus 8 Bytes
     an ein an einem PFS154 angeschlossenes
     Textdisplay

     Uebergabe:
        nr      : Ascii-Codenummer, unter der das
                  Zeichen abrufbar ist
        *userch : Zeiger auf ein 8 Byte grosses
                  Array, das das Bitmap des User-
                  zeichens enthaelt.

     Usage:

             stxt_senduserchar(1, &charbitmap[0]);
             printf("Userzeichen 1: %c",1);
   -------------------------------------------------- */
void stxt_senduserchar(uint8_t nr, const uint8_t *userch)
{
  uint8_t  i, cx;
  uint8_t  ch;
  uint8_t  mask;

  stxt_sendval(0x0400 | nr);

  for (cx= 0; cx<8; cx++)
  {
    ch= *userch;
    userch++;
    stx_set();
    delay_us(500);
    stx_clr();
    delay_us(200);

    mask= 0x80;
    for (i= 0; i< 8; i++)
    {

      if (ch & mask)
      {
        stx_set();
        delay_us(pulselength);
        stx_clr();
        delay_us(pulsepause);
      }
      else
      {
        stx_set();
        delay_us(pulsepause);
        stx_clr();
        delay_us(pulselength);
      }
      mask= mask >> 1;
    }
  }
  stx_set();
  delay(10);

}

/* --------------------------------------------------
                         gotoxy

     Postioniert die Textausgabestelle, an der das
     naechste Zeichen ausgegeben wird

     Uebergabe:
        x,y : Position der Ausgabestelle. 1,1 ent-
              spricht der linken oberen Ecke
   -------------------------------------------------- */
void gotoxy(uint8_t x, uint8_t y)
{
  uint16_t val;

  val= (y << 5) | x | 0x0100;
  stxt_sendval(val);
  delay(1);
  wherex= x;
  wherey= y;
}

/* --------------------------------------------------
     stxt_clrscr

     loescht den Displayinhalt durch Neuinitialisierung
     und setzt die Positionskoordinaten auf 1,1
   -------------------------------------------------- */
void stxt_clrscr(void)
{
  stxt_sendval(0x200);
  wherex= 1; wherey= 1;
  delay(100);
  gotoxy(1,1);
}

/* --------------------------------------------------
     stxt_putchar

     sendet ein Zeichen an den Displayadapter
   -------------------------------------------------- */
void stxt_putchar(uint8_t ch)
{
  stxt_sendval(ch);
  delay_us(2000);
  if (ch == 0x0a)
  {
    if (wherey == 2)
    {
      delay(15);
    }
    else
    {
      wherey++;
    }
    gotoxy(wherex, wherey);
  }
  else
  {
    wherex++;
  }
  if (ch == 0x0d)
  {
    wherex= 1;
    gotoxy(wherex,wherey);
  }
}

#if (enable_stxt_puts == 1)

  /* ------------------------------------------------------------
       stxt_puts

       gibt einen Ascii-Z String aus. Um Benutzerdefinierte
       Zeichen,das Anfuehrungszeichen und das Prozentzeichen
       ausgeben zu koennen, nimmt das Prozentzeichen < % >
       aehnlich einem printfeine Ersetzung bei der Ausgabe durch.

       %0 bis %1 wird durch ein Benutzerdefiniertes an den ent-
       sprechenden Speicherstellen des Zeichens ersetzt.

       %% gibt das Prozentzeichen aus
       %a gibt Anfuehrungsstriche aus
       %l loescht die aktuelle Zeile und springt an deren
          Anfang

       Uebergabe:
         *p : Zeiger auf String

       Usage:


         stxt_puts("Hallo Welt");

         char puffer[] = "PFS154";
         stxt_puts(&puffer[0]);

         // dt. Umlaut 'a' (9. Position im vordefinierten Array)
         // an Speicherstelle 1 setzen
         stxt_addch(9,1);                     // dt. Umlaut a an Speicherstelle 1
         stxt_puts("Z%1hler");
     ------------------------------------------------------------ */
  void stxt_puts(char *p)
  {
    char    ch;
    uint8_t i;

    while(*p)
    {

      if (*p== '%')
      {
        p++;
        if (*p == 0)
        {
          p--;
        }
        else
        {
          ch= *p;
          switch (ch)
          {
            case '%' : stxt_putchar('%'); break;
            case 'l' :
            {
              gotoxy(1, wherey);
              for (i= 0; i< 16; i++) { stxt_putchar(' '); }
              gotoxy(1, wherey);
              break;
            }
            default  : break;
          }
          if ((ch >= '0') && (ch <= '7'))
          {
            stxt_putchar(ch-'0');
          }
        }
      }
      else
      {
        stxt_putchar( *p );
      }
      p++;
    };
  }

#endif

#if (enable_stxt_itoa == 1)

  /* ------------------------------------------------------------
       stxt_itoa

       konvertiert einen Integer zu einem String. Ist Argument
       <komma> != 0 wird ein Kommapunkt in diesen String einge-
       fuegt.

       Uebergabe:
          i       : zu konvertierende Integerzahl
          komma   : Position eines einzufuegenden Kommapunktes
          *puffer : Pufferarray in das die Konvertierung
                    gespeichert wird

       Usage:
         stxt_itoa(12345,2, &puffer[0])

         Im Puffer steht dann der String "123.45", hiermit
         werden Festkommazahlenausgaben moeglich

     ------------------------------------------------------------ */
  void stxt_itoa(int i, char komma, char *puffer)
  {
    typedef enum boolean { FALSE, TRUE }bool_t;

    static int zz[]      = { 10000, 1000, 100, 10 };
    bool_t     not_first = FALSE;

    char       zi;
    int        z, b;

    komma= 5-komma;

    if (!i)
    {
      *puffer= '0';
      puffer++;
      *puffer= 0;
    }
    else
    {
      if(i < 0)
      {
        *puffer= '-';
        puffer++;
        i = -i;
      }
      for(zi = 0; zi < 4; zi++)
      {
        z = 0;
        b = 0;

        if  ((zi==komma) && komma)
        {
          if (!not_first)
          {
            *puffer= '0';
            puffer++;
          }
          *puffer= '.';
          puffer++;
          not_first= TRUE;
        }

        while(z + zz[zi] <= i)
        {
          b++;
          z += zz[zi];
        }
        if(b || not_first)
        {
          *puffer= '0' + b;
          puffer++;
          not_first = TRUE;
        }
        i -= z;
      }
      if (komma== 4)
      {
        *puffer= '.';
        puffer++;
      }
      *puffer= '0' + i;
      puffer++;
      *puffer= 0;
    }
  }



#endif
