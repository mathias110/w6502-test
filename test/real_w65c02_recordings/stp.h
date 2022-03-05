/*
 * 
 *        .org $8000
 *    reset:
 *        stp
 *        nop
 *        nop
 * 
 *        .org $a000
 *    irq:
 *    nmi:
 *        rti
 * 
 *        .org $fffa
 *        .word nmi
 *        .word reset
 *        .word irq
 *    
*/
ProgramFragment stp_prog[] = {
   {0x8000, 3, {0xdb, 0xea, 0xea}},
   {0xa000, 1, {0x40}},
   {0xfffb, 5, {0xa0, 0x00, 0x80, 0x00, 0xa0}},
   {0, 0, {0}}
};
PinState stp_pin[] = {
   {1, 1, 1, 1, 1, 1, 1, 1, 1, 0x8000, 0xdb},
   {1, 1, 1, 1, 1, 0, 1, 1, 1, 0x8001, 0xea},
   {1, 1, 1, 1, 1, 0, 1, 1, 1, 0x8001, 0xea},
   {1, 1, 1, 1, 1, 0, 1, 1, 1, 0x8001, 0xea},
   {1, 1, 1, 1, 1, 0, 1, 1, 1, 0x8001, 0xea},
   {1, 1, 1, 1, 1, 0, 1, 1, 1, 0x8001, 0xea},
   {1, 1, 1, 1, 1, 0, 1, 1, 1, 0x8001, 0xea},
   {1, 1, 1, 1, 1, 0, 1, 1, 1, 0x8001, 0xea},
   {1, 1, 1, 1, 1, 0, 1, 1, 1, 0x8001, 0xea},
   {1, 1, 1, 1, 1, 0, 1, 1, 1, 0x8001, 0xea},
};
const size_t stp_cycles = sizeof(stp_pin)/sizeof(stp_pin[0]);
