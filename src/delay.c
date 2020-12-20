/* ------------------------------------------------------
                         delay.c

     Verzoegerungsschleifen mittels  Warteloops

     Compiler  : SDCC 4.0.3
     MCU       : PFS154 / PFS173

     13.10.2020        R. Seelig

  ------------------------------------------------------- */

#include <stdint.h>
#include "delay.h"


/* ------------------------------------------------------
                       delay_loop_8
        3 cycles per loop, 7 cycles overhead
   ------------------------------------------------------ */
void _delay_loop_8(uint8_t loop_ctr)
{
  loop_ctr; // Ignore unreferenced function argument warning
__asm
  // mov    a, #(loop_ctr)                ; 1 cycle
  // mov    __delay_loop_8_PARM_1, a      ; 1 cycle
  // call   __delay_loop_8                ; 2 cycles
00001$:                                   ; 4 cycles per loop
  dzsn   __delay_loop_8_PARM_1            ;   1 cycle + 1 cycle for final skip
    goto 00001$                           ;   2 cycles
  // ret                                  ; 2 cycles
__endasm;
}

/* ------------------------------------------------------
                       delay_loop_16
         8 cycles per loop, 9 cycles overhead
   ------------------------------------------------------ */
void _delay_loop_16(uint16_t loop_ctr)
{
  loop_ctr; // Ignore unreferenced function argument warning
__asm
  // mov    a, #(loop_ctr)                ; 1 cycle
  // mov    __delay_loop_16_PARM_1+0, a   ; 1 cycle
  // mov    a, #(loop_ctr<<8)             ; 1 cycle
  // mov    __delay_loop_16_PARM_1+1, a   ; 1 cycle
  // call   __delay_loop_16               ; 2 cycles
00001$:                                   ; 8 cycles per loop
  nop                                     ;   1 cycle
  dec   __delay_loop_16_PARM_1+0          ;   1 cycle
  subc  __delay_loop_16_PARM_1+1          ;   1 cycle
  mov   a, __delay_loop_16_PARM_1+0       ;   1 cycle
  or    a, __delay_loop_16_PARM_1+1       ;   1 cycle
  t1sn  f, z                              ;   1 cycle + 1 cycle for final skip
    goto 00001$                           ;   2 cycles
  // ret                                  ; 2 cycles
__endasm;
}

/* ------------------------------------------------------
                       delay_loop_32
          cycles per loop, 13 cycles overhead
   ------------------------------------------------------ */
void _delay_loop_32(uint32_t loop_ctr)
{
  loop_ctr; // Ignore unreferenced function argument warning
__asm
  // mov    a, #(loop_ctr)                ; 1 cycle
  // mov    __delay_loop_32_PARM_1+0, a   ; 1 cycle
  // mov    a, #(loop_ctr<<8)             ; 1 cycle
  // mov    __delay_loop_32_PARM_1+1, a   ; 1 cycle
  // mov    a, #(loop_ctr<<16)            ; 1 cycle
  // mov    __delay_loop_32_PARM_1+1, a   ; 1 cycle
  // mov    a, #(loop_ctr<<24)            ; 1 cycle
  // mov    __delay_loop_32_PARM_1+1, a   ; 1 cycle
  // call   __delay_loop_32               ; 2 cycles
00001$:                                   ; 12 cycles per loop
  nop                                     ;   1 cycle
  dec   __delay_loop_32_PARM_1+0          ;   1 cycle
  subc  __delay_loop_32_PARM_1+1          ;   1 cycle
  subc  __delay_loop_32_PARM_1+2          ;   1 cycle
  subc  __delay_loop_32_PARM_1+3          ;   1 cycle
  mov   a, __delay_loop_32_PARM_1+0       ;   1 cycle
  or    a, __delay_loop_32_PARM_1+1       ;   1 cycle
  or    a, __delay_loop_32_PARM_1+2       ;   1 cycle
  or    a, __delay_loop_32_PARM_1+3       ;   1 cycle
  t1sn  f, z                              ;   1 cycle + 1 cycle for final skip
    goto 00001$                           ;   2 cycles
  // ret                                  ; 2 cycles
__endasm;
}
