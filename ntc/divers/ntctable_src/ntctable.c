/* ----------------------------------------------------------
                         ntctable.c

     erzeugt eine Sourcedatei mit einer Lookup-Tabelle zur
     Auswertung von NTC-Widerstaenden (zur Benutzung in
     Mikrocontrollern)

     20.09.2018    R. Seelig

     Version 0.12   12.11.2019
         Kommandozeilenparameter fuer ADC-Aufloesung in Bit
         hinzugefuegt
   ---------------------------------------------------------- */

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


struct ntcadc
{
  int points;
  int max_adc;
  int adc_aufloes;
  int beta;
  int adc_step;
  int adc_stepmask;
  float pullup;
  float r25;
  uint8_t avr;
};

float errmax_neg = 0.0;
float errmax_pos = 0.0;

/* ----------------------------------------------------------
                           adc2r

     berechnet aus Spannungsteiler bestehend aus Pullup-
     Widerstand und Rx den Widerstand Rx wenn die an ihm
     abfallende Spannung bekannt ist.

     Uebergabe:
        adc_value : Spannungswert an Rx, relativ zur
                    Gesamtspannung (bei Gesamtspannung =
                    1023 Digit entspricht 512 halbe
                    Spannung
        messparam : Struktur auf Parameter wie ADC-Auf-
                    loesung und Digits des ADC
     Rueckgabe:
        Widerstand in Ohm
   ---------------------------------------------------------- */
float adc2r(uint16_t adc_value, struct ntcadc messparam)
{
  return (messparam.pullup * ((float) adc_value)) / ((float)(messparam.max_adc - adc_value));
}

/* ----------------------------------------------------------
                           r2adcvalue
     berechnet aus einem Widerstandswert des Spannungsteilers
     den Wert, den ein ADC messen wird.
   ---------------------------------------------------------- */
int r2adcvalue(float r, struct ntcadc messparam)
{
  return round( (r*(float)messparam.max_adc)/(r+messparam.pullup) );
}

/* ----------------------------------------------------------
                           r2temp
     berechnet aus einem gegebenen Widerstandwertes eines
     NTC die dazugehoerende Temperatur. Hierfuer wird der
     R25 und Beta -wert benoetigt.

     Uebergabe:
           r     : Widerstand in Ohm
       messparam : Struktur auf Parameter, beinhaltet
                   Beta- und R25 Widerstandswert
   ---------------------------------------------------------- */
float r2temp(float r, struct ntcadc messparam)
{
  return (1.0 / ((1.0 / 298.15) + ((1.0 / messparam.beta) * log(r / messparam.r25 )))) -273.15;
}

/* ----------------------------------------------------------
                           temp2r
     berechnet aus einer gegebenen Temperatur eines
     NTC den dazugehoerende Widerstand. Hierfuer wird der
     R25 und Beta -wert benoetigt.

     Uebergabe:
         temp    : Temperatur in Grad Celcius
       messparam : Struktur auf Parameter, beinhaltet
                   Beta- und R25 Widerstandswert
   ---------------------------------------------------------- */
float temp2r(float temp, struct ntcadc messparam)
{
  temp += 273.15;
  return  pow(M_E, messparam.beta*( (1.0/temp)-(1.0/298.15) )) * messparam.r25;
}

/* ----------------------------------------------------------
                           adc2r

     berechnet aus Spannungsteiler bestehend aus Pullup-
     Widerstand und NTC anhand der gemessenen Digits eines
     ADC die dazugehoerende Temperatur

     Uebergabe:
       adc_value : Spannungswert am NTC
       messparam : beinhaltet Pullup, Beta, R25
   ---------------------------------------------------------- */
float adc2temp(uint16_t adc_value, struct ntcadc messparam)
{
  float r;

  r= adc2r(adc_value, messparam);
  return r2temp(r, messparam);
}

/* ----------------------------------------------------------
                           my_log2

     Hilfsfunktion, berechnet den 2er Logarithmus des ange-
     gebenen Werts
   ---------------------------------------------------------- */
uint16_t my_log2(uint16_t value)
{
  return round(log10(value)/log10(2.0));
}

/* ----------------------------------------------------------
                        ntc_gettemp

    berechnet anhand einer Tabelle (wie sie spaeter bspw.
    in einem Mikrocontroller zum Einsatz kommt) und den
    Messparametern die Temperatur.

    Hier nur zur Ueberpruefung und Simulation der Werte
   ---------------------------------------------------------- */
int ntc_gettemp(uint16_t adc_value, struct ntcadc messparam, int *ntctable)
{
  int p1,p2;
  int sf, wanz;

  wanz= messparam.adc_step;
  sf= my_log2(wanz);

  // Stuetzpunkt vor und nach dem ADC Wert ermitteln.

  p1 = ntctable[ (adc_value >> sf)    ];
  p2 = ntctable[ (adc_value >> sf) + 1];

  // zwischen beiden Punkten interpolieren.
  return p1 - ( (p1-p2) * (adc_value & (wanz-1)) ) / wanz;
}

/* ----------------------------------------------------------
                         gentable

     generiert aus den in messparam angegebenen Werten eine
     Lookup-Tabelle fuer Mikrocontroller
   ---------------------------------------------------------- */
void gentable(int *table,struct ntcadc messparam)
{
  float r, temp;
  int i, poi, ti;

  for (i= 0; i< (messparam.points +1); i++)
  {
    temp= adc2temp(i*messparam.adc_step, messparam);
    ti= round(temp * 10);
    table[i]= ti;
  }

  // ersten und letzten Wert korrigieren
  table[0]= table[1] - (table[2] - table[1]);
  poi= messparam.points;
  table[poi]= table[poi-1] - (table[poi-2] - table[poi-1]);

}

/* ----------------------------------------------------------
                         floatabs
         berechnet den Absolutwert eines floats
   ---------------------------------------------------------- */
float floatabs(float value)
{
  return sqrt(value*value);
}

/* ----------------------------------------------------------
                         maxerr_get
     berechnet den maximalen Fehler in Grad Celcius der durch
     die Verwendung der Tabelle entsteht (hinzu kommen Fehler
     des ADC und die des NTC)
   ---------------------------------------------------------- */
float maxerr_get(int *ntctable, struct ntcadc messparam)
{
  float   rt, temp, temp2, maxerr;
  int     i, adc_value;

  temp= 0.0;
  maxerr= 0.0;
  for (i= 0; i< 600; i++)
  {
    rt= temp2r(temp, messparam);
    adc_value= r2adcvalue(rt, messparam);
    temp2=  (ntc_gettemp(adc_value, messparam, ntctable) / 10.0);
    if (floatabs(temp-temp2)> maxerr) maxerr= floatabs(temp-temp2);

//    printf("\nTemp.: %.2f   |   Fehlertemp.: %.2f   |   Abweichung: %.2f", temp, temp2, temp-temp2);

    temp += 0.1;
  }
  return maxerr;
}

/* ----------------------------------------------------------
                         generator

     erstellt aus den in messaram angegebenen Werten und
     einer LookUp table Sourcecode fuer einen Mikrocontroller
   ---------------------------------------------------------- */
int generator(int *table, struct ntcadc messparam)
{
  int   i;
  int   sf, wanz;
  float maxerr;

  wanz= messparam.adc_step;
  sf= my_log2(wanz);

  maxerr= maxerr_get(table, messparam);

  printf("\n");
  printf("\n/* -------------------------------------------------");
  printf("\n     Lookup-table fuer NTC-Widerstand");
  printf("\n     R25-Wert: %.2f kOhm", messparam.r25 / 1000);
  printf("\n     Pullup-Widerstand: %.2f kOhm", messparam.pullup / 1000);
  printf("\n     Materialkonstante beta: %d",messparam.beta);
  printf("\n     Aufloesung des ADC: %d Bit",messparam.adc_aufloes);
  printf("\n     Einheit eines Tabellenwertes: 0.1 Grad Celcius");
  printf("\n     Temperaturfehler der Tabelle: %.1f Grad Celcius",maxerr);
  printf("\n   -------------------------------------------------*/");
  if (messparam.avr)
    printf("\nconst int PROGMEM ntctable[] = {");

  else
    printf("\nconst int ntctable[] = {");

  for (i= 0; i< (messparam.points +1); i++)
  {
    if (!(i % 8)) printf("\n  ");
    printf("%d", table[i]);
    if (i < messparam.points) printf(", ");
  }
  printf("\n};");

  printf("\n");
  printf("\n/* -------------------------------------------------");
  printf("\n                     ntc_gettemp");
  printf("\n");
  printf("\n    zuordnen des Temperaturwertes aus gegebenem");
  printf("\n    ADC-Wert.");
  printf("\n   ------------------------------------------------- */");

  printf("\nint ntc_gettemp(uint16_t adc_value)");
  printf("\n{");
  printf("\n  int p1,p2;");
  printf("\n");
  printf("\n  // Stuetzpunkt vor und nach dem ADC Wert ermitteln.");
  if (messparam.avr)
  {
    printf("\n  p1 = pgm_read_word(&(ntctable[ (adc_value >> %d)    ]));",sf);
    printf("\n  p2 = pgm_read_word(&(ntctable[ (adc_value >> %d) + 1]));",sf);
  }
  else
  {
    printf("\n  p1 = ntctable[ (adc_value >> %d)    ];",sf);
    printf("\n  p2 = ntctable[ (adc_value >> %d) + 1];",sf);
  }
  printf("\n");
  printf("\n  // zwischen beiden Punkten interpolieren.");
  printf("\n  return p1 - ( (p1-p2) * (adc_value & 0x%.4x) ) / %d;", wanz-1, wanz);
  printf("\n}");
  printf("\n");
}

/* ----------------------------------------------------------
                           show_help
     gibt Syntaxmeldung aus
   ---------------------------------------------------------- */
void help_show(void)
{
  printf("  \nntctable 0.12");
  printf("  \n Syntax: ");
  printf("  \n    -r value     | R25 Widerstandswert NTC");
  printf("  \n    -R value     | Popupwiderstand gegen Referenzspannung");
  printf("  \n    -b value     | Betawert NTC (B 25/85)");
  printf("  \n    -A value     | ADC-Aufloesung in Bit");
  printf("  \n    -h           | diese Anzeige (Help)");
  printf("  \n optional: ");
  printf("  \n    -a           | Sourcedatei fuer AVR-Conroller");
  printf("  \n    -l           | Lookup-Table mit 32 Punkten (statt 16)");
  printf("  \n");
}

/* ---------------------------------------------------------------------------
                                    M A I N
   --------------------------------------------------------------------------- */
int main (int argc, char **argv)
{
  int              ntctable[257];
  volatile struct  ntcadc messpara;

  char             tmpstring[100];
  int              c, index;
  int              z;

  // Messparameter mit Standardwerten festlegen (ausser R25- und Pullupwiderstand sowie Betawert)
  messpara.points = 16;
  messpara.max_adc = 0;
  messpara.beta = 0;
  messpara.adc_step = (messpara.max_adc / messpara.points);
  messpara.adc_stepmask = messpara.adc_step - 1;
  messpara.pullup = 0.0;
  messpara.r25 = 0.0;
  messpara.avr = 0;

  // Kommandozeile auswerten
  opterr= 0;

  while ((c = getopt (argc, argv, "alhA:b:r:R:")) != -1)
  {
    switch (c)
    {
      case 'r' :                        // Option fuer R25 Widerstandswert
      {
        strcpy(tmpstring, optarg);
        messpara.r25= atof(tmpstring);
        break;
      }
      case 'R' :                        // Option fuer PullUp-Widerstand
      {
        strcpy(tmpstring, optarg);
        messpara.pullup= atof(tmpstring);
        break;
      }
      case 'b' :                        // Option fuer Betawert
      {
        strcpy(tmpstring, optarg);
        messpara.beta= atoi(tmpstring);
        break;
      }
      case 'a' :                        // AVR Parameter (mit PROGMEM)
      {
        messpara.avr = 1;
        break;
      }
      case 'l' :                        // lange Lookup Tabelle (32 Stuetzpunkte)
      {
        messpara.points = 32;
        break;
      }
      case 'A' :
      {
        strcpy(tmpstring, optarg);
        z= atoi(tmpstring);
        if ((z != 8) && (z != 10) && (z!= 12))
        {
          printf("\n\n Angaben fuer Aufloesung ADC sind nur die Werte 8, 10 oder 12 erlaubt!\n\n");
          return -1;
        }
        messpara.adc_aufloes= z;
        messpara.max_adc= pow(2, z);
        break;
      }
      case 'h' :                        // Hilfe Anzeige
      {
        help_show();
        return -1;
      }
      case '?':
      {
        if (optopt == 'c')
          printf ("Option -%c benoetigt einen Parameter.\n", optopt);
        else if (isprint (optopt))
          printf ("Unbekannte Option `-%c'.\n", optopt);
        else
          printf ("Unbekannter Optionsbezeichner `\\x%x'.\n", optopt);
        return 1;
      }

      default :
      {
        printf("\n\n unbekannter Abbruch \n\n");
        abort();
      }
    }
  }

  messpara.adc_step = (messpara.max_adc / messpara.points);
  messpara.adc_stepmask = messpara.adc_step - 1;


  if ((messpara.r25 == 0.0) || (messpara.pullup == 0.0) || (messpara.beta == 0) || (messpara.max_adc == 0) )
  {
    printf("\n\n mindestens ein benoetigter Parameter wurde nicht angegeben\n");
    help_show();
    return -1;
  }

  gentable(&ntctable[0], messpara);
  generator(&ntctable[0], messpara);


// Testaufruf zur Ueberpruefung der korrekt ermittelten Temperatur
/*
  z=  ntc_gettemp(192, messpara, &ntctable[0]);
  printf("\n\n Temp.: %d \n\n", z);
*/


}
