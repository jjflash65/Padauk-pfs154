#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <stdint.h>
#include <stdio.h>

// Serial TX is on PA7
#define SERIAL_TX_PIN       7

volatile uint16_t txdata;                       // Serial data shift register

void serial_setup(void) {
  // Setup timer2 (TM2) interrupt for 115200 baud
  TM2C = TM2C_CLK_IHRC;                         // Use IHRC -> 16 Mhz
  TM2S = TM2S_PRESCALE_NONE | TM2S_SCALE_DIV2;  // No prescale, scale 2 ~> 8MHz
  TM2B = 69;                                    // Divide by 69 ~> 115942 Hz (apx. 115200)

  PAC |= (1 << SERIAL_TX_PIN);                  // Enable TX Pin as output
  txdata = 0xD55F;                              // Setup 2 stop bits, 0x55 char for autobaud, 1 start bit, 5 stop bits
  INTEN |= INTEN_TM2;                           // Enable TM2 interrupt, send out initial stop bits and autobaud char
}

void serial_irq_handler(void) {
  if (txdata) {                                 // Does txdata contains bits to send?
    if (txdata & 0x01)                          // Check bit (1/0) for sending
      PA |= (1 << SERIAL_TX_PIN);               // Send 1 on TX Pin
    else
      PA &= ~(1 << SERIAL_TX_PIN);               // Send 1 on TX Pin
    txdata >>= 1;                               // Shift txdata
  }
}

void serial_println(char *str) {
  puts(str);
}

int putchar(int c) {
  while (txdata);                               // Wait for completion of previous transmission
  INTEN &= ~INTEN_TM2;                          // Disable TM2 (setup of 16 bit value txdata is non atomic)
  txdata = (c << 1) | 0x200;                    // Setup txdata with start and stop bit
  INTEN |= INTEN_TM2;                           // Enable TM2
  return (c);
}

#endif //__SERIAL_H__
