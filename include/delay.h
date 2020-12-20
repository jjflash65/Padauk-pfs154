/* ------------------------------------------------------
                         delay.h

     Header fuer Verzoegerungsschleifen mittels
     Warteloops

     Compiler  : SDCC 4.0.3
     MCU       : PFS154 / PFS173

     13.10.2020        R. Seelig

  ------------------------------------------------------- */


#ifndef __DELAY_H__
  #define __DELAY_H__

  #if !defined(F_CPU)
    #error "F_CPU is not defined!"
  #endif

  #include <stdint.h>

  #if !defined(MAX)
    #define MAX(a,b) ((a)>(b)?(a):(b))
  #endif

  #define NS_TO_CYCLES(ns)        ((ns)/(1000000000L/F_CPU))
  #define US_TO_CYCLES(us)        (NS_TO_CYCLES(us*1000L))
  #define MS_TO_CYCLES(ms)        (NS_TO_CYCLES(ms*1000000L))

  #define LOOP_CTR(cycles,ovh,lp) (MAX(0,(cycles-ovh-lp)/lp)+1)
  #define LOOP_CTR_8(cycles)      LOOP_CTR(cycles,7,3)
  #define LOOP_CTR_16(cycles)     LOOP_CTR(cycles,9,8)
  #define LOOP_CTR_32(cycles)     LOOP_CTR(cycles,13,12)

  #define _delay_us(us)           \
  	(LOOP_CTR_8(US_TO_CYCLES(us)) < 256L) ? \
  	_delay_loop_8((uint8_t)LOOP_CTR_8(US_TO_CYCLES(us))) : \
  	_delay_loop_16((uint16_t)LOOP_CTR_16(US_TO_CYCLES(us)))

  #define _delay_ms(ms)           \
  	(LOOP_CTR_16(MS_TO_CYCLES(ms)) < 65536L) ? \
  	_delay_loop_16((uint16_t)LOOP_CTR_16(MS_TO_CYCLES(ms))) : \
  	_delay_loop_32((uint32_t)LOOP_CTR_32(MS_TO_CYCLES(ms)))

  #define delay(ms)      _delay_ms(ms)
  #define delay_us(us)   _delay_us(us)

  /* --------------------------------------------------
                       Prototyps
     -------------------------------------------------- */

  void _delay_loop_8(uint8_t loop_ctr);
  void _delay_loop_16(uint16_t loop_ctr);
  void _delay_loop_32(uint32_t loop_ctr);




#endif //__DELAY_H__
