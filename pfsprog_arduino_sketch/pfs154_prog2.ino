/* ---------------------------------------------------------------------
                           pfs154_prog2.ino

     Programmer fuer Mikrocontroller PFS154 von Padauk, basierend
     auf Arduino-UNO Hardware / ATMegaxx8

     zusaetzliche Hardware:
        - Arduino Uno / ATmegaxx8 Board
        - Step-Up Wandler mit MC34063

        - Operationsverstaerker LM358 zur Erzeugung von
          waehlbaren Spannungen
           . Tiefpass an PD5 mit nachgeschaltetem Verstaerker
             mit VU = 2 (analogpin0)
           . dto. an PB1 mit VU = 5.7

     MCU     : atmegaxx8
     F_CPU   : 16000000
     
     Dieses ist an sich kein "Arduino-Programm" im eigentlichen
     Sinne und hat von daher auch keine Funktion >setup> und >loop<,
     sondern ein reines C-Programm, bei dem alle benoetigten
     Funktionen aus einzelnen Softwaremodulen hier eingefuegt sind,
     damit das Arduino-Framework dieses Programm uebersetzen und in
     einen Arduino Uno uebertragen kann

     09.02.2018    R. Seelig
   --------------------------------------------------------------------- */

#include <util/delay.h>
#include <avr/io.h>
#include <avr/eeprom.h>


/* ---------------------------------------------------------------------
                            allgemeine Defines
   --------------------------------------------------------------------- */
// damit innerhalb dieses Programms Ausgaben wie mit einem
// gewoehnlichen >printf< funktionieren
#define tiny_printf(str,...)  (my_printf(PSTR(str), ## __VA_ARGS__))
#define printf        tiny_printf

#define adc_low       ADCL
#define adc_high      ADCH
#define BAUDRATE      115200

/* ---------------------------------------------------------------------
                     Defines fuer die Anschluesse
   --------------------------------------------------------------------- */

// Bitmasken
// ----------------------------------------------
#define MASK0                ( 1 << 0 )
#define MASK1                ( 1 << 1 )
#define MASK2                ( 1 << 2 )
#define MASK3                ( 1 << 3 )
#define MASK4                ( 1 << 4 )
#define MASK5                ( 1 << 5 )
#define MASK6                ( 1 << 6 )
#define MASK7                ( 1 << 7 )

// PD5 ist alternate Function von "Output Counter2 Match B"  (OC0B)
// vdd_pin Verbindung zu OP mit R2/R1 = 4.7 / 4.7k = Gain 2
#define vdd_pin_init()     ( DDRD |= MASK5 )
float gain_vdd = 2.0;

// PB1 ist alternate Function von "Output Counter1 Match A"  (OC1A)
// vpp_pin Verbindung zu OP mit R2/R1 = 4.7 / 1k = Gain 5.7
#define vpp_pin_init()     ( DDRB |= MASK1 )
float gain_vpp = 5.7;

// PD3 ist alternate Function von "Output Counter2 Match B"  (OC2B)
#define dcdcpin_init()     ( DDRD |= MASK3 )

// PB0 ist Aktivitaets_LED
#define led_init()         ( DDRB |= MASK0 )
#define led_set()          ( PORTB |= MASK0 )
#define led_clr()          ( PORTB &= ~MASK0 )

// PC1 Jumper-Pin zum Aktivieren der Kalibrierungsfunktion
#define calib_init()       { DDRC &= ~MASK1; PORTC |= MASK1; }
#define is_calib()         (!(( PINC & MASK1) >> 1 ))

// I/O Leitungen fuer PD2 - SDA (PFS-PA6)
//                und PD6 - SCK (PFS-PA3) des PFS-Targets
#define sda_out_init()     ( DDRD |= MASK2 )

#define sda_in_init()      { DDRD &= ~MASK2; PORTD |= MASK2; }
#define sda_set()          ( PORTD |= MASK2 )
#define sda_clr()          ( PORTD &= ~MASK2 )
#define is_sda()           ( (PIND & MASK2) >> 1 )

#define sck_out_init()     ( DDRD |= MASK6 )
#define sck_set()          ( PORTD |= MASK6 )
#define sck_clr()          ( PORTD &= ~MASK6 )
#define is_sck()           ( (PIND & MASK6) >> 1 )

/* ---------------------------------------------------------------------
                     Defines fuer internes EEProm
   --------------------------------------------------------------------- */
#define eep_adr_byte       0x10
#define eep_adr_gain_vdd   0x14
#define eep_adr_gain_vpp   0x18


/* ---------------------------------------------------------------------
                       Konstante fuer PS154 - Device
   --------------------------------------------------------------------- */
const uint8_t  device_datalen   = 14;
const uint8_t  device_adrlen    = 13;
const float    device_vpp_read  = 5.5;
const float    device_vpp_write = 7.6;
const float    device_vpp_erase = 8.2;
const uint16_t device_memend    = 0x7ff;
const uint16_t device_id        = 0xaa1;

/* ---------------------------------------------------------------------
                                Variable
   --------------------------------------------------------------------- */
// Datentransfer
uint8_t  blockmem[512];
uint16_t blksize = 500;
uint8_t  blkanz;

// Hoehe der eigenen Betriebsspannung, wird beim Start durch Messung der
// eigenen Betriebsspannung ueberschrieben
float own_ub = 4.5;

// fuer own_printf
char printfkomma = 1;


/* ------------------------------------------------------------------------------
                                     Definitionen
   ------------------------------------------------------------------------------ */

/* #####################################################################
                       UART - serielle Schnittstelle
   ##################################################################### */

/* --------------------------------------------------
    Initialisierung der seriellen Schnittstelle:

    Protokoll: 8 Daten-, 1 Stopbit
   -------------------------------------------------- */
void uart_init(void)
{
  uint16_t ubrr;

  //  Code, fuer Baudraten groesser 57600
  if (BAUDRATE> 57600)
  {
    ubrr= (F_CPU/16/(BAUDRATE>>1));
    ubrr--;
    UCSR0A |= 1<<U2X0;                                  // Baudrate verdoppeln
  }
  else
  {
    ubrr= (F_CPU/16/BAUDRATE-1);
  }

  UBRR0H = (uint8_t)(ubrr>>8);                    // Baudrate setzen
  UBRR0L = (uint8_t)ubrr;

  UCSR0B = (1<<RXEN0)|(1<<TXEN0);                       // Transmitter und Receiver enable
  UCSR0C = (3<<UCSZ00);                                 // 8 Datenbit, 1 Stopbit
}

/* --------------------------------------------------
                   uart_gethex
     liest ein Hex-Byte von der seriellen Schnitt-
     stelle ein und gibt dieses als gelesenen
     Funktionswert zurueck
   -------------------------------------------------- */
uint8_t uart_gethex(void)
{
  uint8_t ch;
  uint8_t value;

  ch= uart_getchar();
  if (ch > 'F')
  {
    ch= (ch-'a')+10;
  }
  else
  {
    if (ch> '9') ch= (ch-'A')+10; else ch -= '0';
  }
  value= (ch<< 4);
  ch= uart_getchar();

  if (ch > 'F')
  {
    ch= (ch-'a')+10;
  }
  else
  {
    if (ch> '9') ch= (ch-'A')+10; else ch -= '0';
  }
  value |= ch;
  return value;
}

/* --------------------------------------------------
                   uart_gethexword

     liest ein Hex-Word von der seriellen Schnitt-
     stelle ein und gibt dieses als gelesenen
     Funktionswert zurueck
   -------------------------------------------------- */
uint16_t uart_gethexword(void)
{
  uint16_t i, i2;

  i= uart_gethex();
  i= i & 0x00ff;
  i2= uart_gethex();
  i2= i2 & 0x00ff;
  i2= i2 | (i << 8);
  return i2;
}

/* --------------------------------------------------
                    uart_putchar
    Zeichen ueber die serielle Schnittstelle senden
   -------------------------------------------------- */

void uart_putchar(uint8_t ch)
{
  while (!( UCSR0A & (1<<UDRE0)));                      // warten bis Transmitterpuffer leer ist
  UDR0 = ch;                                            // Zeichen senden
}

/* --------------------------------------------------
                     uart_ischar
    testen, ob ein Zeichen auf der Schnittstelle
    ansteht
   -------------------------------------------------- */
uint8_t uart_ischar( void )
{
  return (UCSR0A & (1<<RXC0));
}

/* --------------------------------------------------
                      uart_getchar
    Zeichen von serieller Schnittstelle lesen
   -------------------------------------------------- */
uint8_t uart_getchar( void )
{
  while(!(UCSR0A & (1<<RXC0)));                         // warten bis Zeichen eintrifft
  return UDR0;
}

/* --------------------------------------------------
                       uart_readint

     liest einen 16-Bit signed Integer auf der seriellen
     Schnittstelle ein (allerdings reicht der Eingabebereich
     nur von -32767 .. +32767).

     Korrektur ist mit der Loeschtaste nach links moeglich.
   -------------------------------------------------- */
int16_t uart_readint(void)
{
  uint8_t   signflag= 0;
  uint16_t  sum = 0;
  uint8_t   zif;
  uint8_t   ch;

  do
  {
    ch= uart_getchar();
    if (ch== 0x0a) ch= 0x0d;

    // Ziffern auswerten
    if ((ch>= '0') && (ch<= '9'))
    {
      zif= ch-'0';

      if ((sum== 0) && (zif))        // erste Ziffer
      {
        sum= zif;
        uart_putchar(ch);
      }
      else
      if (sum < 3277)
      {
        if  (!( ((sum * 10) > 32750) && (zif > 7) ))
        {
          sum= (sum*10) + zif;
          uart_putchar(ch);
        }
      }
    }

    // letzte Eingabe loeschen
    if ((ch== 127) || (ch== 8))    // letzte Eingabe loeschen
    {
      if (sum)
      {
        sum /= 10;
        uart_putchar(8);
        uart_putchar(' ');
        uart_putchar(8);
      }
      else
      if (signflag)
      {
        uart_putchar(8);
        uart_putchar(' ');
        uart_putchar(8);
        signflag= 0;
      }
    }

    // Eingabe Minuszeichen
    if ((ch== '-') && (sum == 0))
    {
      signflag= 1;
        uart_putchar('-');
    }

  } while (ch != 0x0d);              // wiederholen bis Returnzeichen eintrifft
  if (signflag) return sum *(-1); else return sum;
}
// --------------- -Ende UART serielle Schnittstelle  ------------------

/* #####################################################################
                     PWM und Ausgabe analoger Spannungen
   ##################################################################### */

/* -------------------------------------------------
                    pwm_vpp_init

     initialisiert 16-Bit Timer1 als PWM-Analog-
     ausgang.

     (benoetigt Tiefpassfilter an PIN PB1)
   ------------------------------------------------- */
void pwm_vpp_init(void)
{
   vpp_pin_init();                          // PB1 ist Ausgang fuer 16-Bit PWM

   // WGM10= 0 ; WGM11 WGM12, WGM13 = 1 ===> Modus 14 (Fast PWM)
   // Prescaler wird durch CS10, CS11, CS12 (1.0.0) gesetzt (keine Taktvorteilung)

   TCCR1A = (1<<COM1A1) | (1<<WGM11);
   TCCR1B = (1<<WGM12) | (1<<WGM13) | (1<<CS10);
}

/* -------------------------------------------------
                   pwm_vpp_set

     setzt das Gesamtdauer - Pulsdauer Verhaeltnis
     der PWM auf Timer/Counter1
   ------------------------------------------------- */
void pwm_vpp_set(uint16_t tim1, uint16_t tim2)
{
  // tim1 ==> Gesamtdauer
  // tim2 ==> Pulsdauer

  ICR1H = (tim1>>8);
  ICR1L = (uint8_t) tim1 & 0x00ff;
  OCR1AH = (tim2>>8);
  OCR1AL = (uint8_t) tim2 & 0x00ff;
}

/* -------------------------------------------------
                     vpp_set

     setzt ein Gesamtdauer - Pulse Verhaeltnis, das
     einer analogen Spannung bei einer Rechteck-
     spannungshoehe von refwert entspricht.
     Spannungausgang hier PB1
   ------------------------------------------------- */
void vpp_set(float value)
{
  float    r;

  value /= gain_vpp;
  r= (value * 1024.0) / own_ub;

  pwm_vpp_set(1024,(int)r);

}

/* -------------------------------------------------
                      pwm_vdd_init

     initialisiert 8-Bit Timer0 als PWM-Analog-
     ausgang.

     (benoetigt Tiefpassfilter an PIN PD5)
   ------------------------------------------------- */
void pwm_vdd_init(void)
{
  // Setzt OC0B zu Beginn, setzt zurueck bei Erreichen von Wert in OCR0B
  vdd_pin_init();

  TCCR0A = (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);

  // Zaehlertakt= F_CPU / 1
  TCCR0B = (1 << CS00) | (1 << WGM02); ;

  OCR0A = 255;
  OCR0B = 128;
}

/* -------------------------------------------------
                      vdd_set

     setzt ein Gesamtdauer - Pulse Verhaeltnis, das
     einer analogen Spannung bei einer Rechteck-
     spannungshoehe von refwert entspricht.
     Spannungausgang hier PD5
   ------------------------------------------------- */
void vdd_set(float value)
{
  float    r;

  value /= gain_vdd;
  r= (value * 255.0) / own_ub;
  OCR0B= (uint8_t)r;
}

/* -------------------------------------------------
                      pwm_dcdc_init

     setzt eine PWM mit 25 kHz (bei F_CPU = 16MHz)
     und einer Tastgrad Puls / Gesamt von
     OCR2B / 80.

     Dies ist die Steuerfrequenz eines einfachen
     Step-Up Wandlers (DC-DC)
   ------------------------------------------------- */
void pwm_dcdc_init(void)
{
  // Setzt OC2B zu Beginn, setzt zurueck bei Erreichen von Wert in OCR2B
  dcdcpin_init();

  TCCR2A = (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);

  // Zaehlertakt= F_CPU / 8 => bei F_CPU = 16MHz ist Zaehlfreq. = 2 MHz
  TCCR2B = (1 << CS21) | (1 << WGM22); ;

  // Gesamtdauer von 80 * (1/2MHz) = 0.04 ms => 25 kHz
/*
  OCR2A = 80;
  OCR2B = 58;             // (0.04 ms / 80) * OCR2B = Pulsdauer
                          // 18 * (0.04ms / 80) = 9 us Pulsdauer
*/
  OCR2A = 20;
  OCR2B = 15;
}
// ---------- Ende PWM und Ausgabe analoger Spannungen -----------------

/* #####################################################################
                                   ADC
   ##################################################################### */

/* -------------------------------------------------
                     adc_get10bit
       liest den ADC mit dem in "adc_init" ausge-
       waehlten Kanal aus

       Rueckgabe : gelesener Analogwert
   ------------------------------------------------- */
unsigned int adc_get10bit (void)
{
  unsigned int adcwert,adch;

  adcwert= adc_low;
  adch= adc_high;
  adch = adch<<8;
  adcwert|= adch;
  return(adcwert);
}

/* ------------------------------------------------
                      adc_getm10bit
       liest den ADC 8 mal aus und bildet den Mittel-
       wert hiervon.
   ------------------------------------------------- */
int adc_getm10bit(void)
{
  int i,addwert;

  addwert= 0;
  for (i= 0; i< 8; i++)
  {
    addwert= adc_get10bit()+ addwert;
    _delay_ms(30);
  }
  return (addwert >> 3);    // addwert = addwert / 8;
}

/* -------------------------------------------------
                      adc_init

   <refmode> bezeichnet die zu verwendende Referenz
     0 : externe Referenz an Aref
     1 : +AVcc mit ext. Kondensator
     2 : reserviert
     3 : interne Referenz: 1,1V ATmega168 / 2,56V ATmega8

   <channel> bezeichnet den Kanal auf dem gemessen werden soll

   Spannungsreferenz ATMEGA48 / 88 / 168 / 328:

       Bit: 7      6
refmode   REFS1  REFS0      Referenz

    0       0      0        ext. Referenz an  Aref
    1       0      1        +AVcc mit ext. Kondensator
    2       1      0        reserviert
    3       1      1        Interne Spannungsreferenz: 1,1V (ATmegaxx8) / 2,56V (ATmega8)

  Bit5: ADLAR = 0 (rechtsbuendig); MUX= 0 => ADC0 gewaehlt

Bit3..0: Selektor Analogmultiplexer


   // ADMUX= 1<<REFS0;                                          // AVcc (Betriebsspannung) als Referenz
   // ADMUX= 1<<REFS0 | 1<<REFS1;                               // int. Referenz
   ------------------------------------------------- */
void adc_init(uint8_t refmode, uint8_t channel)
{

   ADMUX = refmode << 6;
   ADMUX |= channel;

   ADCSRA= 0xe7;            // ADC enable, single gestartet, free running mode
                        // Taktteiler / 128
   _delay_ms(1);
   ADCSRA= 0xa7;
}

/* ------------------------------------------------
                   adc_getvoltage

     misst die ueber einen Spannungsteiler am
     durch adc_init gewaehlten Eingang anliegende
     Spannung
   ------------------------------------------------ */
float adc_getvoltage(float r1, float r2)
{
  float    ubf;

  ubf= (1.1 * adc_getm10bit()) / 1024;
  return ((r1+r2)/r2)*ubf;
}
// -------------------------- Ende ADC  --------------------------------

/* #####################################################################
                   Funktionen fuer Ausgabe rund um my_printf
   ##################################################################### */
/* ------------------------------------------------------------
                            putint
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
    uart_putchar('0');
  }
  else
  {
    if(i < 0)
    {
      uart_putchar('-');
      i = -i;
    }

    for(zi = 0; zi < 4; zi++)
    {
      z = 0;
      b = 0;

      if  ((zi== komma) && komma)
      {
        if (!not_first) uart_putchar('0');
        uart_putchar('.');
        not_first= TRUE;
      }

      while(z + zz[zi] <= i)
      {
        b++;
        z += zz[zi];
      }
      if(b || not_first)
      {
        uart_putchar('0' + b);
        not_first = TRUE;
      }
      i -= z;
    }
    if (komma== 4) uart_putchar('.');
    uart_putchar('0' + i);
  }
}

/* --------------------------------------------------
                    hexnibbleout

     gibt die unteren 4 Bits eines chars als Hexa-
     ziffer aus. Eine Pruefung ob die oberen vier
     Bits geloescht sind erfolgt NICHT !
  --------------------------------------------------  */
void hexnibbleout(uint8_t b)
{
  if (b< 10) b+= '0'; else b+= 55;
  uart_putchar(b);
}

/* --------------------------------------------------
                       puthex

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
                  my_putramstring

     gibt einen String aus dem RAM aus
   -------------------------------------------------- */
void my_putramstring(uint8_t *p)
{
  do
  {
    uart_putchar( *p );
  } while( *p++);
}

/* --------------------------------------------------
                       my_printf

     alternativer Ersatz fuer printf.

     Aufruf:

         my_printf(PSTR("Ergebnis= %d"),zahl);

     oder durch define-Makro (besser):
         #define tiny_printf(str,...)  (my_printf(PSTR(str), ## __VA_ARGS__))

         tiny_printf("Ergebnis= %d",zahl);

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
void my_printf(const uint8_t *s,...)
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
          uart_putchar(arg1);
          break;
        }
        case '%':
        {
          uart_putchar(token);
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
      uart_putchar(ch);
    }
    s++;
  }while (ch != '\0');
}

// ------------------- Ende Textausgabe my_printf  ---------------------

/* ------------------------------------------------
                     calibrate

     stellt Ausgangsspannungen UNCALIBRIERT ein,
     und ermittelt durch eine Benutzereingabe den
     Calibrierwert der Verstaerker.

     Calibrierdaten werden im int. EEProm ab
     Adresse eepadr (0x10) gespeichert.

     Hinweis: Calibrierdaten gehen mit einem neuen
     Flashvorgang verloren
   ------------------------------------------------ */
void calibrate(void)
{
  uint16_t w;
  float    gvdd, gvpp;

  vpp_set(8.0);
  vdd_set(6.0);
  printf("\n\n\r Calibrate the PWM to analog output\n\r");
  printf(      " ----------------------------------\n\n\r");
  printf("enter the voltage at Vpp Pin (PA5) [mv]: ");
  w= uart_readint();
  gvpp= (float)w / 1000.0;
  gain_vpp= gain_vpp * gvpp / 8.0;
  vpp_set(8.0);

  printf("\n\renter the voltage at Vdd Pin [mv]      : ");
  w= uart_readint();
  gvdd= (float)w / 1000.0;
  gain_vdd= gain_vdd*gvdd / 6.0;
  vdd_set(6.0);

  // Kalibrierdaten ins EEProm schreiben

  eeprom_write_byte((uint8_t*)eep_adr_byte, 0xaa);        // Kennung, dass kalibriert wurde

  eeprom_write_float((float*)eep_adr_gain_vpp, gain_vpp);    // Verstaerkungsfaktor OP-Amp 1
  eeprom_write_float((float*)eep_adr_gain_vdd, gain_vdd);    // Verstaerkungsfaktor OP-Amp 0

  printf("\n\n\rremove calibration jumper and reset...");
  while(1);
}

/* ------------------------------------------------
                    testvoltage_set

     Ueberpruefen der Funktionsfaehigkeit der
     anwaehlbaren Spannungen fuer Vpp und Vdd.

     Anwahl der Spannungen ueber UART
   ------------------------------------------------ */
void testvoltage_set(void)
{
  uint16_t w;
  float    ft;

  vpp_set(5.0);
  vdd_set(3.3);
  while(1)
  {
    printf("\n\r  enter a voltage to set at pin7 OP-Amp. [mv]: ");
    w= uart_readint();
    ft= (float)w / 1000.0;
    vpp_set(ft);
    printf("\n\r  enter a voltage to set at pin1 OP-Amp. [mv]: ");
    w= uart_readint();
    ft= (float)w / 1000.0;
    vdd_set(ft);
  }
}

/* #####################################################################
                Funktionen zum Handling mit dem Target-Device
   ##################################################################### */
   
/* ------------------------------------------------
                     pfs_init

     Initialiserung der I/O Pins und der Spannung
     Vdd
   ------------------------------------------------ */
void pfs_init(void)
{
  vdd_set(0.05);        // 0.05V == false an Vdd

  sck_out_init();
  sck_clr();

  sda_in_init();       // SDA-Leitung auf Eingang

  _delay_ms(1);
}

/* ------------------------------------------------
                     pfs_sendword

     sendet maximal 16 Bits (Word) an das Target

     Uebergabe:
         word   : zu sendendes Datum
         length : Anzahl zu sendender Bits inner-
                  halb des Datums
   ------------------------------------------------ */
void pfs_sendword(uint16_t word, uint8_t length)
{
  uint8_t i;

  word= word << (16-length);
  sda_out_init();

  // serielle Datumsausgabe
  for (i= 0; i < length; i++)
  {
    sck_clr();
    if (word & 0x8000) sda_set(); else sda_clr();
    word <<= 1;
    _delay_us(3);
    sck_set();
    _delay_us(3);
  }

  sck_clr();
  sda_in_init();

}

/* ------------------------------------------------
                     pfs_receiveword

     Liest ein Datum mit der Bitlaenge "length"
     vom Target ein
   ------------------------------------------------ */
uint16_t pfs_receiveword(uint8_t length)
{
  uint8_t  i;
  uint16_t word = 0;

  for (i= 0; i < length; i++)
  {
    sck_clr();
    _delay_us(3);
    sck_set();
    _delay_us(2);

    word <<= 1;
    if (is_sda()) word |= 0x01;        // lesen nach dem Delay um das erste Bit zu fixen
    _delay_us(1);
  }

  sck_clr();
  _delay_us(2);
  sck_set();                           // Lesezugriffe haben einen zusaetzlichen Taktzyklus, warum ?
  _delay_us(2);
  sck_clr();
  _delay_us(2);

  return word;
}

/* ------------------------------------------------
                     pfs_readword

     liest einen Wert aus dem Speicher
     Hinweis: Read-Mode muss aktiv sein
   ------------------------------------------------ */
uint16_t pfs_readword(uint16_t address)
{
  pfs_sendword(address, device_adrlen);
  return pfs_receiveword(device_datalen);
}


/* ------------------------------------------------
                    pfs_enterpgmmode

     aktiviert den Programm-Modus

     Werte fuer cmd:
         0xa3 : erase mode
         0xa6 : read mode
         0xa7 : write mode

     Rueckgabe:
         DeviceID, fuer PFS154 == 0xaa1
         jeder andere Wert als 0xaa1 zeigt an,
         dass das Aktivieren des Programmiermodus
         fehlgeschlagen ist
   ------------------------------------------------ */
uint16_t pfs_enterpgmmode(uint8_t cmd)
{
  pfs_init();

  vdd_set(3.3);
  _delay_ms(1);
  vpp_set(device_vpp_read);
  _delay_ms(10);

  vdd_set(0.05);           // bool = 0 => reset
  _delay_us(500);
  _delay_ms(5);
  vdd_set(3.3);             // bool = 1
  _delay_us(500);
  _delay_ms(5);
  pfs_sendword(0xA5A5,16);
  pfs_sendword(0xA5A5,8);
  pfs_sendword(cmd,8);

  pfs_sendword(0x00, 3);                        // 3 Fuellbits senden
  return pfs_receiveword(13) & 0xfff;
}

/* ------------------------------------------------
                pfs_read_device_id_seq

     liest die Device ID aus und gibt diese als
     Funktionsargument zurueck
   ------------------------------------------------ */
uint16_t pfs_read_device_id_seq(void)
{
  uint16_t DeviceID;
  DeviceID = pfs_enterpgmmode(0xA7);            // write mode

  pfs_init();
  _delay_ms(10);

  return DeviceID;
}

/* ------------------------------------------------
                     pfs_erasedevice

     loescht den Flashspeicher des Targets
   ------------------------------------------------ */
void pfs_erasedevice(void)
{
  uint16_t DeviceID;
  uint8_t i;
  uint8_t ch;

  DeviceID= 0x00;
  DeviceID= pfs_enterpgmmode(0xa3);                      // Aktivierung erase-mode
//  printf(" Erase init response: %x\n", DeviceID);

  vpp_set(device_vpp_erase);
  _delay_ms(10);
  vdd_set(2.0);
  _delay_ms(10);
  sda_out_init();
  sda_clr();

  for (i= 0; i < 2; i++)
  {
    sck_set();
    _delay_us(5000);
    sck_clr();
    _delay_us(2);
    sck_set();
    _delay_us(2);
    sck_clr();
    _delay_us(2);
  }

  sck_set();
  _delay_us(2);
  sck_clr();
  _delay_us(2);

  pfs_init();
  _delay_ms(10);
}

/* ------------------------------------------------
                     pfs_writewords

     schreibt 2 Words ins Target, Schreibmodus
     muss aktiviert sein
   ------------------------------------------------ */
void pfs_writewords(uint16_t word1, uint16_t word2, uint16_t address)
{
  uint8_t i;

  if (address & 0x0002)                   // schreibt entweder den Upper oder Lower
                                          // Teil der Page zur gleichen Zeit
  {
    pfs_sendword(0xffff , device_datalen);
    pfs_sendword(0xffff , device_datalen);
    pfs_sendword(word1  , device_datalen);
    pfs_sendword(word2  , device_datalen);
  }
  else
  {
    pfs_sendword(word1  , device_datalen);
    pfs_sendword(word2  , device_datalen);
    pfs_sendword(0xffff , device_datalen);
    pfs_sendword(0xffff , device_datalen);
  }
  pfs_sendword(address & 0xfffc, device_adrlen);

  sda_out_init();
  _delay_us(1);

  sda_set();                            //  SDA togglen, wird im
                                        // Originalprogrammer so gemacht (lt. Tim / CPUCPLD )
  _delay_us(1);
  sda_clr();
  _delay_us(1);

  for (i= 0; i < 8; i++)
  {
    sck_set();
    _delay_us(22);
    sck_clr();
    _delay_us(22);
  }
  _delay_us(1);

  sda_in_init();
  pfs_sendword(0,1);                     // fuehrende Null senden
}

// ------------------ Handling Target-Device Ende  ---------------------

/* ----------------------------------------------------------------------------------
                                    M A I N
   ---------------------------------------------------------------------------------- */
int main(void)
{
  int       i;
  uint8_t   b, ch;
  uint16_t  DeviceID;
  uint16_t  proglen;
  uint16_t  w1, w2;
  uint16_t  pc;
  uint16_t  mcx;

  _delay_ms(150);
  uart_init();
  calib_init();

  adc_init(3,2);
  pwm_vpp_init();
  pwm_vdd_init();

  led_init();

  led_set();
  _delay_ms(150);
  led_clr();
  _delay_ms(150);

//  pwm_dcdc_init();

  own_ub= (adc_getvoltage(10.0, 1.0));
  vpp_set(0.03);
  vdd_set(0.03);


// ---------------------------------------------------
//   Kalibrierung der Ausgangsspannungen Vpp und Vdd
//   ==> Kommentierung aufheben nicht vergessen !!
// ---------------------------------------------------


  // Kalibrieren wenn Kalibrierjumper gesteckt
  if (is_calib()) { calibrate(); }


  // wenn Kalibrierdaten vorhanden sind, diese lesen
  b= eeprom_read_byte((uint8_t*)eep_adr_byte);
  if (b== 0xaa)
  {
    gain_vpp= eeprom_read_float((float*)eep_adr_gain_vpp);
    gain_vdd= eeprom_read_float((float*)eep_adr_gain_vdd);
  }

  while(1)
  {
    do
    {
      ch= uart_getchar();
    } while ((ch != 'P') && (ch != 'R') && (ch != 'r'));

    switch (ch)
    {
      // Programm device
      case 'P' :
      {
        printfkomma= 3;

        vpp_set(0.02);
        vdd_set(0.02);
        _delay_ms(40);

        pfs_init();

        DeviceID= pfs_read_device_id_seq();
        printf("0x%x\r\n", DeviceID);
        proglen= uart_gethexword();
        printf("%x\r", proglen);

        // Anzahl Datenbloecke ermitteln
        blkanz= proglen / blksize;
        // ist die Programmlaenge nicht ein exaktes Vielfaches von blksize,
        // einen Block mehr erwarten
        if (proglen % blksize) blkanz++;

        led_set();
        pfs_init();
        _delay_ms(20);
        pfs_erasedevice();
        _delay_ms(20);

        DeviceID= pfs_enterpgmmode(0xa7);                    // write mode

        vdd_set(5.8);
        _delay_ms(10);
        vpp_set(device_vpp_write);
        _delay_ms(10);

        pc= 0;
        for (i= 0; i< blkanz; i++)                    // Anzahl Bloecke
        {
          for (mcx= 0; mcx< blksize; mcx++)           // einen Speicherblock empfangen
          {
            blockmem[mcx]= uart_getchar();
          }

          for (mcx= 0; mcx< blksize; mcx += 4)        // und diesen flashen
          {
            w1= (blockmem[mcx] << 8) | blockmem[mcx+1];
            w2= (blockmem[mcx+2] << 8) | blockmem[mcx+3];

            if (pc< proglen)
            {
              pfs_writewords(w1, w2, pc);
            }

            pc +=2;
          }
          uart_putchar('x');
        }

        pfs_init();
        vpp_set(0.05);
        vdd_set(0.03);
        led_clr();
        break;
      }
      // Run device
      case 'R' :
      {
        pfs_init();
        vpp_set(0.05);
        vdd_set(0.03);
        _delay_ms(100);
        vdd_set(5.0);
        break;
      }

      // stop running device
      case 'r' :
      {
        pfs_init();
        vpp_set(0.05);
        vdd_set(0.03);
        break;
      }
      default : break;

    }
  }
}
