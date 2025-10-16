/* -----------------------------------------------------
                        oled1306_i2c.h

    Header fuer das Anbinden eines OLED Displays mit
    SSD1306 Controller und I2C Interface

     MCU   : PFS154 / 173


     Pinbelegung I2C (siehe i2c.h)
     -----------------------------

     PA5 = SDA
     PA6 = SCL


     18.12.2020 R. Seelig
  ------------------------------------------------------ */

/*
                                                             V   G   S   S
                                                             c   N   C   D
                                                             c   D   L   A
             +----------------+                     +-----------------------------+
         PB4 | 1   |___|   16 | PB3                 |        o   o   o   o        |
         PB5 | 2           15 | PB2                 |                             |
         PB6 | 3           14 | PB1                 |   -----------------------   |
         PB7 | 4  PFS154   13 | PB0                 |  |                       |  |
             -------------                          |  |    SSD1306 - OLED     |  |
         VDD | 5 (1)   (8) 12 | GND                 |  |    128 x 64 Pixel     |  |
         PA7 | 6 (2)   (7) 11 | PA0                 |  |                       |  |
 ICPDA / PA6 | 7 (3)   (6) 10 | PA4                 |  |         I2C           |  |
   Vpp / PA5 | 8 (4)   (5)  9 | PA3 / ICPCK         |   ---+---------------+---   |
             +----------------+                     |      |               |      |
                                                    |      +---------------+      |
                                                    +-----------------------------+

*/


#ifndef in_i2c_devices
  #define in_i2c_devices

  #include <stdint.h>
  #include "pfs1xx_gpio.h"
  #include "delay.h"

  #include "i2c.h"


  #define ssd1306_addr       0x78

  #define prints             oled_prints

  // um Speicherplatz zu sparen koennen Kleinbuchstaben vom Einbetten
  // in das Kompilat ausgeschlossen werden

  #define smallch_enable        1              // 1 : Kleinbuchstaben verfuegbar
                                               // 0 : nicht verfuegbar

  extern uint8_t aktxp;
  extern uint8_t aktyp;
  extern uint8_t doublechar;
  extern uint8_t bkcolor;
  extern uint8_t textcolor;

/* -----------------------------------------------------
                       Prototypen
   ----------------------------------------------------- */

  void ssd1306_init(void);
  void gotoxy(uint8_t x, uint8_t y);
  void clrscr(void);
  void oled_putchar(uint8_t ch);
  void oled_prints(uint8_t *c);

#endif
