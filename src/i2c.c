/* -----------------------------------------------------
                        sw_i2c.c

    Softwareimplementierung  I2C-Bus (Bitbanging)

      Compiler  : SDCC 4.0.3
      MCU       : PFS154 / PFS173

    14.10.2020   R. Seelig
  ------------------------------------------------------ */

#include "i2c.h"

// --------------------------------------------------------------------
//                      Prototypen aus sw_i2c.h
// --------------------------------------------------------------------

/* -------------------------------------------------------

                   i2c_master_init

    setzt die Pins die fuer den I2C Bus verwendet werden
    als Ausgaenge


                   i2c_sendstart(void)

    erzeugt die Startcondition auf dem I2C Bus


                   i2c_start(uint8_t addr)

    erzeugt die Startcondition auf dem I2C Bus und
    schreibt eine Deviceadresse auf den Bus


                    i2c_stop(void)

    erzeugt die Stopcondition auf dem I2C Bus


               i2c_write_nack(uint8_t data)
   schreibt einen Wert auf dem I2C Bus OHNE ein Ack-
   nowledge einzulesen


                i2c_write(uint8_t data)
   schreibt einen Wert auf dem I2C Bus.

   Rueckgabe:
               > 0 wenn Slave ein Acknowledge gegeben hat
               == 0 wenn kein Acknowledge vom Slave
                    i2c_read(uint8_t ack)

   liest ein Byte vom I2c Bus.

   Uebergabe:
               1 : nach dem Lesen wird dem Slave ein
                   Acknowledge gesendet
               0 : es wird kein Acknowledge gesendet

   Rueckgabe:
               gelesenes Byte
   -------------------------------------------------------

void i2c_master_init(void);
void i2c_sendstart(void);
uint8_t i2c_start(uint8_t addr);
void i2c_stop();
void i2c_write_nack(uint8_t data);
uint8_t i2c_write(uint8_t data);
uint8_t i2c_read(uint8_t ack);
#define i2c_read_ack()    i2c_read(1)
#define i2c_read_nack()   i2c_read(0)

*/

uint8_t I2C_CX;                // universelle Zaehlvariable, aus RAM
                               // Speicherplatzgruenden hier ausnahmsweise
                               // global

uint8_t ACK;


/* ---------------------------------------------------------
                           i2c_delay
       an die "Reaktionszeiten" der Register des STM8 an-
       gepasste Warteschleife
   --------------------------------------------------------- */
void i2c_delay(uint8_t anz)
{
  volatile uint8_t count;

  for (count= 0; count< anz; count++)
  {
    _delay_us(1);
  }
}

/* #################################################################
     Funktionen fuer I2C - Bus (Softwareimplementierung)
   ################################################################# */

/* -------------------------------------------------------
                   i2c_master_init

    setzt die Pins die fuer den I2C Bus verwendet werden
    als Ausgaenge
   ------------------------------------------------------- */
void i2c_master_init()
{
  i2c_sda_hi();
  i2c_scl_hi();
}

/* -------------------------------------------------------
                     i2c_sendstart(void)
    erzeugt die Startcondition auf dem I2C Bus
   ------------------------------------------------------- */
void i2c_sendstart(void)
{
  i2c_scl_hi();
  long_del();

  i2c_sda_lo();
  long_del();
}

/* -------------------------------------------------------
                     i2c_start
    erzeugt die Startcondition und sendet anschliessend
    die Deviceadresse
   ------------------------------------------------------- */
uint8_t i2c_start(uint8_t addr)
{
//  uint8_t ack;

  i2c_sendstart();
  ACK= i2c_write(addr);
  return ACK;
}

/* -------------------------------------------------------
                   i2c_startaddr

   startet den I2C-Bus und sendet Bauteileadresse.
   rwflag bestimmt, ob das Device beschrieben oder
   gelesen werden soll
  -------------------------------------------------- */
void i2c_startaddr(uint8_t addr, uint8_t rwflag)
{
  addr = (addr << 1) | rwflag;

  i2c_sendstart();
  i2c_write(addr);
}

/* -------------------------------------------------------
                     i2c_stop
    erzeugt die Stopcondition auf dem I2C Bus
   ------------------------------------------------------- */
void i2c_stop(void)
{
   i2c_sda_lo();
   long_del();
   i2c_scl_hi();
   short_del();
   i2c_sda_hi();
   long_del();
}

/* -------------------------------------------------------
                   i2c_write_nack(uint8_t data)

   schreibt einen Wert auf dem I2C Bus OHNE ein Ack-
   nowledge einzulesen
  ------------------------------------------------------- */
void i2c_write_nack(uint8_t data)
{

  for(I2C_CX= 0; I2C_CX < 8; I2C_CX++)
  {
    i2c_scl_lo();
    short_del();

    if(data & 0x80) { i2c_sda_hi(); }
               else { i2c_sda_lo(); }

    short_del();

    i2c_scl_hi();
    short_del();
    wait_del();

    data= data << 1;
  }
  i2c_scl_lo();
  short_del();
}


/* -------------------------------------------------------
                   i2c_write(uint8_t data)

   schreibt einen Wert auf dem I2C Bus.

   Rueckgabe:
               > 0 wenn Slave ein Acknowledge gegeben hat
               == 0 wenn kein Acknowledge vom Slave
   ------------------------------------------------------- */
uint8_t i2c_write(uint8_t data)
{
//   uint8_t ack;

   i2c_write_nack(data);

  //  9. Taktimpuls (Ack)

  i2c_sda_hi();
  long_del();

  i2c_scl_hi();
  long_del();

  i2c_sda_hi();
  long_del();

  if (i2c_is_sda()) ACK= 0; else ACK= 1;

  i2c_scl_lo();
  long_del();

  return ACK;
}

/* -------------------------------------------------------
                   i2c_write16(uint8_t data)

   schreibt einen 16-Bit Wert auf dem I2C Bus.

   Rueckgabe:
               > 0 wenn Slave ein Acknowledge gegeben hat
               == 0 wenn kein Acknowledge vom Slave
   ------------------------------------------------------- */
uint8_t i2c_write16(uint16_t data)
{
//  uint8_t ack;

  ACK= i2c_write(data >> 8);
  if (!(ACK)) return 0;
  ACK= i2c_write(data & 0xff);

  return ACK;
}

/* -------------------------------------------------------
                    i2c_read(uint8_t ack)

   liest ein Byte vom I2c Bus.

   Uebergabe:
               1 : nach dem Lesen wird dem Slave ein
                   Acknowledge gesendet
               0 : es wird kein Acknowledge gesendet

   Rueckgabe:
               gelesenes Byte
   ------------------------------------------------------- */
uint8_t i2c_read(uint8_t ack)
{
  uint8_t data= 0x00;

  for(I2C_CX= 0; I2C_CX < 8; I2C_CX++)
  {
    i2c_scl_lo();
    short_del();
    i2c_scl_hi();

    short_del();
    i2c_sda_hi();
    wait_del();

    if(i2c_is_sda()) data|= (0x80 >> I2C_CX);
  }

  i2c_scl_lo();

  i2c_sda_hi();

  long_del();

  if (ack)
  {
    i2c_sda_lo();
    long_del();
  }

  i2c_scl_hi();
  long_del();

  i2c_scl_lo();
  long_del();

  i2c_sda_hi();

  return data;
}
