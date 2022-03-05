/*
 * 
 *      .org $7ff0
 *  bra_1:
 *      bra bra_2
 * 
 *      .org $8000
 *  reset:
 *      bra bra_1
 *  bra_2:
 *      bra bra_3
 *      nop
 *      nop
 *  bra_3:
 *      nop
 * 
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
ProgramFragment bra_prog[] = {
   {0x7ff0, 2, {0x80, 0x10}},
   {0x8000, 7, {0x80, 0xee, 0x80, 0x02, 0xea, 0xea, 0xea}},
   {0xa000, 1, {0x40}},
   {0xfffb, 5, {0xa0, 0x00, 0x80, 0x00, 0xa0}},
   {0, 0, {0}}
};
PinState bra_pin[] = {
   {1, 1, 1, 1, 1, 1, 1, 1, 1, 0x8000, 0x80},
   {1, 1, 1, 1, 1, 0, 1, 1, 1, 0x8001, 0xee},
   {1, 1, 1, 1, 1, 0, 1, 1, 1, 0x8002, 0x80},
   {1, 1, 1, 1, 1, 0, 1, 1, 1, 0x8002, 0x80},
   {1, 1, 1, 1, 1, 1, 1, 1, 1, 0x7ff0, 0x80},
   {1, 1, 1, 1, 1, 0, 1, 1, 1, 0x7ff1, 0x10},
   {1, 1, 1, 1, 1, 0, 1, 1, 1, 0x7ff2, 0x00},
   {1, 1, 1, 1, 1, 0, 1, 1, 1, 0x7ff2, 0x00},
   {1, 1, 1, 1, 1, 1, 1, 1, 1, 0x8002, 0x80},
   {1, 1, 1, 1, 1, 0, 1, 1, 1, 0x8003, 0x02},
   {1, 1, 1, 1, 1, 0, 1, 1, 1, 0x8004, 0xea},
   {1, 1, 1, 1, 1, 1, 1, 1, 1, 0x8006, 0xea},
};
const size_t bra_cycles = sizeof(bra_pin)/sizeof(bra_pin[0]);
