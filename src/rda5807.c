/* -----------------------------------------------------
                          rda5807.h

    Softwaremodul fuer grundsaetzliche Steuerungs-
    funktionen zum UKW-Receiver RDA5807 mit I2C
    Schnittstelle

      Compiler  : SDCC 4.0.3
      MCU       : PFS154 / PFS173

    21.10.2020   R. Seelig
  ------------------------------------------------------ */


#include "rda5807.h"

uint16_t aktfreq =   1018;              // Startfrequenz ( 101.8 MHz )
uint8_t  aktvol  =   3;                 // Startlautstaerke

// nachfolgende Variable sind als globale Variable ausgefuehrt, weil bei
// der Uebergabe an eine Funktion ein erhoehter Rambedarf festgestellt
// wurde und der Ram im PFS so knapp ist (nur 128 Byte) dass dieser Ram
// dringend eingespaart werden muss.

volatile uint16_t rda5807_reg[7];


uint16_t tmpfreq;
uint8_t  tmpvol;

uint8_t rda5807_cx;                     // universelle Zaehlvariable, aus RAM
                                        // Speicherplatzgruenden hier ausnahmsweise
                                        // global

const uint16_t rda5807_regdef[7] = {
            0x0758,                     // 00 default ID
            0x0000,                     // 01 reserved
            0xF009,                     // 02 DHIZ, DMUTE, BASS, POWERUPENABLE, RDS
            0x0000,                     // 03 reserved
            0x1400,                     // 04 SOFTMUTE
            0x84DF,                     // 05 INT_MODE, SEEKTH=0110, unbekannt, Volume=15
            0x4000 };                   // 06 OPENMODE=01

/* --------------------------------------------------
      rda5807_writereg

   einzelnes Register des RDA5807 schreiben, als
   Macro ausgefuehrt, um bei Uebergabe RAM zu
   sparen (weil Flashspeicher genuegend vorhanden
   ist).
   -------------------------------------------------- */
#define rda5807_writereg(reg)                 \
  { i2c_start(rda5807_adrr);                  \
    i2c_write(reg);                           \
    i2c_write16(rda5807_reg[reg]);            \
    i2c_stop(); }


/* --------------------------------------------------
      rda5807_write

   alle Register es RDA5807 schreiben
   -------------------------------------------------- */
void rda5807_write(void)
{
  i2c_start(rda5807_adrs);
  for (rda5807_cx= 2; rda5807_cx< 7; rda5807_cx++)
  {
    i2c_write16(rda5807_reg[rda5807_cx]);
  }
  i2c_stop();
}

/* --------------------------------------------------
      rda5807_reset
   -------------------------------------------------- */
void rda5807_reset(void)
{
  for (rda5807_cx= 0; rda5807_cx< 7; rda5807_cx++)
  {
    rda5807_reg[rda5807_cx]= rda5807_regdef[rda5807_cx];
  }
  rda5807_reg[2]= rda5807_reg[2] | 0x0002;    // Enable SoftReset
  rda5807_write();
  rda5807_reg[2]= rda5807_reg[2] & 0xFFFB;    // Disable SoftReset
}

/* --------------------------------------------------
      rda5807_poweron
   -------------------------------------------------- */
void rda5807_poweron(void)
{
  rda5807_reg[3]= rda5807_reg[3] | 0x010;   // Enable Tuning
  rda5807_reg[2]= rda5807_reg[2] | 0x001;   // Enable PowerOn

  rda5807_write();

  rda5807_reg[3]= rda5807_reg[3] & 0xFFEF;  // Disable Tuning
}

/* --------------------------------------------------
      rda5807_setfreq

      setzt angegebene Frequenz * 0.1 MHz

      Bsp.:
         rda5807_setfreq(1018);    // setzt 101.8 MHz
                                   // die neue Welle
   -------------------------------------------------- */
int rda5807_setfreq()
{
  uint16_t channel;

  channel= aktfreq;

  channel -= fbandmin;
  channel&= 0x03FF;
  rda5807_reg[3]= (channel << 6) + 0x10;  // Channel + TUNE-Bit + Band=00(87-108) + Space=00(100kHz)

  i2c_start(rda5807_adrs);
  i2c_write16(0xD009);
  i2c_write16(rda5807_reg[3]);
  i2c_stop();

  delay(100);
  return 0;
}

/* --------------------------------------------------
      rda5807_setvol
   -------------------------------------------------- */
void rda5807_setvol(void)
{
  rda5807_reg[5]=(rda5807_reg[5] & 0xFFF0) |  aktvol;
  rda5807_writereg(5);
}

/* --------------------------------------------------
      rda5807_setmono
   -------------------------------------------------- */
void rda5807_setmono(void)
{
  rda5807_reg[2]=(rda5807_reg[2] | 0x2000);
  rda5807_writereg(2);
}

/* --------------------------------------------------
      rda5807_setstero
   -------------------------------------------------- */
void rda5807_setstereo(void)
{
  rda5807_reg[2]=(rda5807_reg[2] & 0xdfff);
  rda5807_writereg(2);
}

/* --------------------------------------------------
                   rda5807_getsig

     liefert Empfangsstaerke des eingestellten
     Senders zurueck
   -------------------------------------------------- */
uint8_t rda5807_getsig(void)
{
  uint8_t b;

  delay(100);
  i2c_start(rda5807_adrs | 1);
  for (rda5807_cx= 0; rda5807_cx < 3; rda5807_cx++)
  {
    b= i2c_read_ack();
    delay(5);
    if (rda5807_cx == 2)
    {
      i2c_read_nack();
      i2c_stop();
      return b;
    }
  }
  b= i2c_read_nack();

  i2c_stop();
  return b;
}

/* --------------------------------------------------
                   rda5807_scandown

     automatischer Sendersuchlauf, Frequenz
     dekrementieren
   -------------------------------------------------- */
void rda5807_scandown(void)
{
  tmpvol= aktvol;
  rda5807_setvol();

  if (aktfreq== fbandmin) { aktfreq= fbandmax; }
  do
  {
    aktfreq--;
    rda5807_setfreq();
    rda5807_cx= rda5807_getsig();
  }while ((rda5807_cx < sigschwelle) && (aktfreq > fbandmin));

  aktvol= tmpvol;
  rda5807_setvol();
}

/* --------------------------------------------------
                   rda5807_scanup

     automatischer Sendersuchlauf, Frequenz
     inkrementieren
   -------------------------------------------------- */
void rda5807_scanup(void)
{
  uint8_t siglev;

  tmpvol= aktvol;
  rda5807_setvol();

  if (aktfreq== fbandmax) { aktfreq= fbandmin; }
  do
  {
    aktfreq++;
    rda5807_setfreq();
    siglev= rda5807_getsig();
  }while ((siglev < sigschwelle) && (aktfreq < fbandmax));

  aktvol= tmpvol;
  rda5807_setvol();

}
