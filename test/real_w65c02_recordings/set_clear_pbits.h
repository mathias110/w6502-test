/*
 * 
 *      .org $8000
 *  reset:
 *      clc
 *      cld
 *      cli
 *      clv
 *      sec
 *      sed
 *      sei
 *      nop
 *      .org $a000
 *  irq:
 *  nmi:
 *      rti
 * 
 *      .org $fffa
 *      .word nmi
 *      .word reset
 *      .word irq
 *  
*/
ProgramFragment set_clear_pbits_prog[] = {
   {0x8000, 8, {0x18, 0xd8, 0x58, 0xb8, 0x38, 0xf8, 0x78, 0xea}},
   {0xa000, 1, {0x40}},
   {0xfffb, 5, {0xa0, 0x00, 0x80, 0x00, 0xa0}},
   {0, 0, {0}}
};
PinState set_clear_pbits_pin[] = {
   {1, 1, 1, 1, 1, 1, 1, 1, 1, 0x8000, 0x18},
   {1, 1, 1, 1, 1, 0, 1, 1, 1, 0x8001, 0xd8},
   {1, 1, 1, 1, 1, 1, 1, 1, 1, 0x8001, 0xd8},
   {1, 1, 1, 1, 1, 0, 1, 1, 1, 0x8002, 0x58},
   {1, 1, 1, 1, 1, 1, 1, 1, 1, 0x8002, 0x58},
   {1, 1, 1, 1, 1, 0, 1, 1, 1, 0x8003, 0xb8},
   {1, 1, 1, 1, 1, 1, 1, 1, 1, 0x8003, 0xb8},
   {1, 1, 1, 1, 1, 0, 1, 1, 1, 0x8004, 0x38},
   {1, 1, 1, 1, 1, 1, 1, 1, 1, 0x8004, 0x38},
   {1, 1, 1, 1, 1, 0, 1, 1, 1, 0x8005, 0xf8},
   {1, 1, 1, 1, 1, 1, 1, 1, 1, 0x8005, 0xf8},
   {1, 1, 1, 1, 1, 0, 1, 1, 1, 0x8006, 0x78},
   {1, 1, 1, 1, 1, 1, 1, 1, 1, 0x8006, 0x78},
   {1, 1, 1, 1, 1, 0, 1, 1, 1, 0x8007, 0xea},
   {1, 1, 1, 1, 1, 1, 1, 1, 1, 0x8007, 0xea},
};
const size_t set_clear_pbits_cycles = sizeof(set_clear_pbits_pin)/sizeof(set_clear_pbits_pin[0]);
