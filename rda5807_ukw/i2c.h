/* -----------------------------------------------------
                          i2c.h

    Header fuer Softwareimplementierung des I2C-Buses
    (Bitbanging)

      Compiler  : SDCC 4.0.3
      MCU       : PFS154 / PFS173

    14.10.2020   R. Seelig
  ------------------------------------------------------ */


#ifndef in_sw_i2c
  #define in_sw_i2c

  #include <stdint.h>
  #include "pfs1xx_gpio.h"
  #include "delay.h"

  #define config_i2c_pa     1
  #define config_i2c_pb     0

  #if (config_i2c_pb == 1)

    // Dataanschluss
    #define i2c_sda_hi()      PB7_input_init()
    #define i2c_sda_lo()      { PB7_output_init(); PB7_clr(); }
    #define i2c_is_sda()      is_PB7()

    // Clockanschluss
    #define i2c_scl_hi()      PB6_input_init()
    #define i2c_scl_lo()      { PB6_output_init(); PB6_clr(); }

  #endif

  #if (config_i2c_pa == 1)

    // Dataanschluss
    #define i2c_sda_hi()      PA5_input_init()
    #define i2c_sda_lo()      { PA5_output_init(); PA5_clr(); }
    #define i2c_is_sda()      is_PA5()

    // Clockanschluss
    #define i2c_scl_hi()      PA6_input_init()
    #define i2c_scl_lo()      { PA6_output_init(); PA6_clr(); }

  #endif

  #define short_puls     1            // Einheiten fuer einen langen Taktimpuls
  #define long_puls      1            // Einheiten fuer einen kurzen Taktimpuls
  #define del_wait       1            // Wartezeit fuer garantierten 0 Pegel SCL-Leitung

  #define short_del()     i2c_delay(short_puls)
  #define long_del()      i2c_delay(long_puls)
  #define wait_del()      i2c_delay(del_wait)

  // --------------------------------------------------------------------
  //                      Prototypenbeschreibung
  // --------------------------------------------------------------------

  /* -------------------------------------------------------

      ############### i2c_master_init ##############

      setzt die Pins die fuer den I2C Bus verwendet werden
      als Ausgaenge


      ############## i2c_sendstart(void) ###############

      erzeugt die Startcondition auf dem I2C Bus


      ############## i2c_start(uint8_t addr) ##############

      erzeugt die Startcondition auf dem I2C Bus und
      schreibt eine Deviceadresse auf den Bus


      ############## i2c_stop(void) ##############

      erzeugt die Stopcondition auf dem I2C Bus


      ############## i2c_write_nack(uint8_t data) ##############

      schreibt einen Wert auf dem I2C Bus OHNE ein Ack-
      nowledge einzulesen


      ############## i2c_write(uint8_t data) ##############

      schreibt einen Wert auf dem I2C Bus.

      Rueckgabe:
                 > 0 wenn Slave ein Acknowledge gegeben hat
                 == 0 wenn kein Acknowledge vom Slave


      ############## i2c_write16(uint16_t data) ##############

      schreibt einen 16 Bit Wert (2Bytes) auf dem I2C Bus.

      Rueckgabe:
                 > 0 wenn Slave ein Acknowledge gegeben hat
                 == 0 wenn kein Acknowledge vom Slave


      ############## i2c_read(uint8_t ack) ##############

      liest ein Byte vom I2c Bus.

      Uebergabe:
                 1 : nach dem Lesen wird dem Slave ein
                     Acknowledge gesendet
                 0 : es wird kein Acknowledge gesendet

      Rueckgabe:
                  gelesenes Byte
     ------------------------------------------------------- */

  void i2c_master_init(void);
  void i2c_sendstart(void);
  uint8_t i2c_start(uint8_t addr);
  void i2c_stop();
  void i2c_startaddr(uint8_t addr, uint8_t rwflag);
  void i2c_write_nack(uint8_t data);
  uint8_t i2c_write(uint8_t data);
  uint8_t i2c_write16(uint16_t data);
  uint8_t i2c_read(uint8_t ack);

  #define i2c_read_ack()    i2c_read(1)
  #define i2c_read_nack()   i2c_read(0)


#endif
