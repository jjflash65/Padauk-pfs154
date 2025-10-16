/* ------------------------------------------------------------------
                            tm1650.c

     Softwaremodul zum Ansprechen des TM1650  Treiberbaustein fuer
     7-Segmentanzeigen

     MCU   : PFS154

     Pinbelegung siehe tm1650.h

     CLK und DIO sind mit jeweils 2,2 kOhm Pop-Up Wider-
     staenden nach +5V zu versehen

     08.11.2024 R. Seelig
   ------------------------------------------------------------------ */

/*

    Segmentbelegung der Anzeige:

        a
       ---
    f | g | b            Segment |  a  |  b  |  c  |  d  |  e  |  f  |  g  | Doppelpunkt (nur fuer POS1) |
       ---               ---------------------------------------------------------------------------------
    e |   | c            Bit-Nr. |  0  |  1  |  2  |  3  |  4  |  5  |  6  |              7              |
       ---
        d


    Bit 7 der 7-Segmentanzeige ist der Dezimalpunkt
*/

#include "tm1650.h"

/* ----------------------------------------------------------
                     Globale Variable
   ---------------------------------------------------------- */

uint8_t    hellig    = 7;                // beinhaltet Wert fuer die Helligkeit (erlaubt: 0x00 .. 0x0f);

uint8_t    led7sbmp[17] =                // Bitmapmuster fuer Ziffern von 0 .. F
                { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07,
                  0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71,
                  0x00 };

/*  ------------------- Kommunikation -----------------------

    Der Treiberbaustein tm1650 wird etwas "merkwuerdig
    angesprochen. Er verwendet zur Kommunikation ein I2C
    Protokoll, jedoch OHNE eine Adressvergabe. Der Chip ist
    somit IMMER angesprochen. Aus diesem Grund wird die
    Kommunikation mittels Bitbanging vorgenommen. Hierfuer
    kann jeder freie I/O Anschluss des Controllers verwendet
    werden (siehe defines am Anfang).
   ---------------------------------------------------------- */

void tm1650_start(void)              // I2C Bus-Start
{
  bb_scl_hi();
  bb_sda_hi();
  puls_len();
  bb_sda_lo();
}

void tm1650_stop(void)               // I2C Bus-Stop
{
  bb_scl_lo();
  puls_len();
  bb_sda_lo();
  puls_len();
  bb_scl_hi();
  puls_len();
  bb_sda_hi();
}

void tm1650_write (uint8_t value)    // I2C Bus-Datentransfer
{
  uint8_t i;

  for (i = 0; i <8; i++)
  {
    bb_scl_lo();
    if (value & 0x80) { bb_sda_hi(); }
                   else { bb_sda_lo(); }
    puls_len();
    value = value << 1;
    bb_scl_hi();
    puls_len();
  }
  bb_scl_lo();
  puls_len();                        // der Einfachheit wegen wird ACK nicht abgefragt
  bb_scl_hi();
  puls_len();
  bb_scl_lo();

}

/*  ----------------------------------------------------------
                      Benutzerfunktionen
    ---------------------------------------------------------- */

/*  ----------------------------------------------------------
                         tm1650_setbright

       setzt die Helligkeit der Anzeige
       erlaubte Werte fuer Value sind 0..7
    ---------------------------------------------------------- */
void tm1650_setbright(uint8_t value)
{
  tm1650_start();
  tm1650_write(0x48);                     // DATA Command fuer LED-Treiber

  //  7:5  => Helligkeit
  //    4  => 0 = 7-Digit-LED ; 1 = 8-Digit-LED
  //    0  => 0 = Display aus ; 1 = Display an
  tm1650_write((value << 5) | 0x11);      // unteres Nibble beinhaltet Helligkeitswert
  tm1650_stop();
}

/*  ----------------------------------------------------------
                        tm1650_selectpos

        waehlt die zu beschreibende Anzeigeposition aus
     --------------------------------------------------------- */
void tm1650_selectpos(char nr)
{
  nr= 3-nr;
  tm1650_start();
  tm1650_write(0x40);                // Auswahl LED-Register
  tm1650_stop();
  tm1650_setbright(hellig);

  tm1650_start();
  tm1650_write(0x68 | (nr << 1));           // Auswahl der 7-Segmentanzeige
}

/*  ----------------------------------------------------------
                          tm1650_clear

       loescht die Anzeige auf dem Modul
    --------------------------------------------------------- */
void tm1650_clear(void)
{
  uint8_t i;

  for(i=0; i<4; i++)
  {
    tm1650_selectpos(i);
    tm1650_write(0x00);
  }
  tm1650_stop();

}

/*  ----------------------------------------------------------
                          tm1650_setbmp

       gibt ein Bitmapmuster an einer Position aus
    --------------------------------------------------------- */
void tm1650_setbmp(uint8_t pos, uint8_t value)
{
  tm1650_selectpos(pos);             // zu beschreibende Anzeige waehlen

  tm1650_write(value);               // Bitmuster value auf 7-Segmentanzeige ausgeben
  tm1650_stop();

}

/*  ----------------------------------------------------------
                          tm1650_setzif

       gibt ein Ziffer an einer Position aus
       Anmerkung: das Bitmuster der Ziffern ist in
                  led7sbmp definiert
    --------------------------------------------------------- */
void tm1650_setzif(uint8_t pos, uint8_t zif)
{
  tm1650_selectpos(pos);             // zu beschreibende Anzeige waehlen

  zif= led7sbmp[zif];
  tm1650_write(zif);               // Bitmuster value auf 7-Segmentanzeige ausgeben
  tm1650_stop();

}

/*  ----------------------------------------------------------
                          tm1650_setseg

       setzt ein einzelnes Segment einer Anzeige

       pos: Anzeigeposition (0..3)
       seg: das einzelne Segment (0..7 siehe oben)
    --------------------------------------------------------- */
void tm1650_setseg(uint8_t pos, uint8_t seg)
{

  tm1650_selectpos(pos);             // zu beschreibende Anzeige waehlen
  tm1650_write(1 << seg);
  tm1650_stop();
}

#if (setdez_enable == 1)

  /*  ----------------------------------------------------------
                            tm1650_setdez

         gibt einen 4-stelligen dezimalen Wert auf der
         Anzeige aus

         value  : anzuzeigender Wert
      --------------------------------------------------------- */
  void tm1650_setdez(int value, uint8_t dp)
  {
    uint8_t i,v,bmp;

    for (i= 4; i> 0; i--)
    {
      v= value % 10;
      bmp= led7sbmp[v];
      if (((i-1) == (3-dp)) && (dp > 0)) bmp |= 0x80;
      tm1650_setbmp(i-1, bmp);
      value= value / 10;
    }
  }
#endif

#if (sethex_enable == 1)
  /*  ---------------------------------------------------------
                              sethex

         gibt einen 4-stelligen hexadezimalen Wert auf der
         Anzeige aus
      --------------------------------------------------------- */
  void tm1650_sethex(uint16_t value)
  {
    uint8_t i,v;

    for (i= 4; i> 0; i--)
    {
      v= value % 0x10;
      tm1650_setbmp(i-1, led7sbmp[v]);
      value= value / 0x10;
    }
  }
#endif

/*  ----------------------------------------------------------
                          tm1650_init

       initialisiert die Anschluesse des Controllers zur
       Kommunikation als Ausganege und loescht die Anzeige
    ---------------------------------------------------------- */
void tm1650_init(void)
{
  scl_init();
  sda_init();
  tm1650_clear();
}
