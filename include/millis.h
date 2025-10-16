#ifndef __MILLIS_H__
#define __MILLIS_H__

#include <stdint.h>

#define US_PER_TICK      32

volatile uint8_t _ticks;                        // Increments every 32 uS
volatile uint32_t _millis;                      // Number of elapsed milliseconds (rolls over every 49.71 days)

void millis_setup() {
  // Setup timer16 (T16) interrupt to tick every 32 uS
  // Note: This assumes the internal IHRC oscillator is running and has been calibrated to the normal 16MHz and not some weird frequency.
  // The system clock can still be set to use a divider, or to use ILRC or EOSC (as long as IHRC is still enabled).
  T16M = (uint8_t)(T16M_CLK_IHRC | T16M_CLK_DIV1 | T16M_INTSRC_8BIT);
  T16C = 0;
  INTEN |= INTEN_T16;
}

uint32_t millis() {
  INTEN &= ~INTEN_T16;                          // Disable T16 (read of 32 bit value _millis is non-atomic)
  uint32_t currentMillis = _millis;             // Read _millis while T16 interrupt is off (otherwise it might be modified on us!)
  INTEN |= INTEN_T16;                           // Re-enable T16
  return currentMillis;
}

void millis_irq_handler() {
  if (_ticks++ == (uint8_t)(1000/US_PER_TICK)) {  // 1000/32 is actually 31.25, so we are off a bit here (0.8%)
    _ticks = 0;
    _millis++;
  }
}

#endif //__MILLIS_H__
