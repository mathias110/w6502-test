#define CHIPS_UTIL_IMPL
#include "util/w65c02dasm.h"
#include "utest.h"
#include <string.h>
#include <ctype.h>

#define T(b) ASSERT_TRUE(b)
#define TOP(str) T(op(str))

typedef struct {
    const uint8_t* ptr;
    const uint8_t* end_ptr;
    uint16_t pc;
    size_t str_pos;
    char str[32];
} ctx_t;
static ctx_t ctx;

static void init(uint16_t pc, const uint8_t* ptr, size_t len) {
    ctx.ptr = ptr;
    ctx.end_ptr = ptr + len;
    ctx.pc = pc;
    ctx.str_pos = 0;
    memset(ctx.str, 0, sizeof(ctx.str));
}

static uint8_t in_cb(void* user_data) {
    (void)user_data;
    if (ctx.ptr < ctx.end_ptr) {
        return *ctx.ptr++;
    }
    else {
        return 0;
    }
}

static void out_cb(char c, void* user_data) {
    (void)user_data;
    if ((ctx.str_pos + 1) < sizeof(ctx.str)) {
        ctx.str[ctx.str_pos++] = c;
        ctx.str[ctx.str_pos] = 0;
    }
}

static bool op(const char* res) {
    ctx.str_pos = 0;
    ctx.pc = w65c02dasm_op(ctx.pc, in_cb, out_cb, 0);
    for (int i = 0; i < 32; i++) {
        ctx.str[i] = toupper(ctx.str[i]);
    }
    return 0 == strcmp(ctx.str, res);
}

UTEST(w65c02dasm, LDA) {
    uint8_t prog[] = {
        // immediate
        0xA9, 0x00,         // LDA #$00
        0xA9, 0x01,         // LDA #$01
        0xA9, 0x00,         // LDA #$00
        0xA9, 0x80,         // LDA #$80

        // zero page
        0xA5, 0x02,         // LDA $02
        0xA5, 0x03,         // LDA $03
        0xA5, 0x80,         // LDA $80
        0xA5, 0xFF,         // LDA $FF

        // absolute
        0xAD, 0x00, 0x10,   // LDA $1000
        0xAD, 0xFF, 0xFF,   // LDA $FFFF
        0xAD, 0x21, 0x00,   // LDA $0021

        // zero page,X
        0xA2, 0x0F,         // LDX #$0F
        0xB5, 0x10,         // LDA $10,X    => 0x1F
        0xB5, 0xF8,         // LDA $F8,X    => 0x07
        0xB5, 0x78,         // LDA $78,X    => 0x87

        // absolute,X
        0xBD, 0xF1, 0x0F,   // LDA $0x0FF1,X    => 0x1000
        0xBD, 0xF0, 0xFF,   // LDA $0xFFF0,X    => 0xFFFF
        0xBD, 0x12, 0x00,   // LDA $0x0012,X    => 0x0021

        // absolute,Y
        0xA0, 0xF0,         // LDY #$F0
        0xB9, 0x10, 0x0F,   // LDA $0x0F10,Y    => 0x1000
        0xB9, 0x0F, 0xFF,   // LDA $0xFF0F,Y    => 0xFFFF
        0xB9, 0x31, 0xFF,   // LDA $0xFF31,Y    => 0x0021

        // indirect,X
        0xA1, 0xF0,         // LDA ($F0,X)  => 0xFF, second byte in 0x00 => 0x1234
        0xA1, 0x70,         // LDA ($70,X)  => 0x70 => 0x4321

        // indirect,Y
        0xB1, 0xFF,         // LDA ($FF),Y  => 0x1234+0xF0 => 0x1324
        0xB1, 0x7F,         // LDA ($7F),Y  => 0x4321+0xF0 => 0x4411

        // indirect zp
        0xB2, 0x78,         // LDA ($78)
    };
    init(0, prog, sizeof(prog));
    TOP("LDA #$00");
    TOP("LDA #$01");
    TOP("LDA #$00");
    TOP("LDA #$80");
    TOP("LDA $02");
    TOP("LDA $03");
    TOP("LDA $80");
    TOP("LDA $FF");
    TOP("LDA $1000");
    TOP("LDA $FFFF");
    TOP("LDA $0021");
    TOP("LDX #$0F");
    TOP("LDA $10,X");
    TOP("LDA $F8,X");
    TOP("LDA $78,X");
    TOP("LDA $0FF1,X");
    TOP("LDA $FFF0,X");
    TOP("LDA $0012,X");
    TOP("LDY #$F0");
    TOP("LDA $0F10,Y");
    TOP("LDA $FF0F,Y");
    TOP("LDA $FF31,Y");
    TOP("LDA ($F0,X)");
    TOP("LDA ($70,X)");
    TOP("LDA ($FF),Y");
    TOP("LDA ($7F),Y");
    TOP("LDA ($78)");
}

UTEST(w65c02dasm, LDX) {
    uint8_t prog[] = {
        // immediate
        0xA2, 0x00,         // LDX #$00
        0xA2, 0x01,         // LDX #$01
        0xA2, 0x00,         // LDX #$00
        0xA2, 0x80,         // LDX #$80

        // zero page
        0xA6, 0x02,         // LDX $02
        0xA6, 0x03,         // LDX $03
        0xA6, 0x80,         // LDX $80
        0xA6, 0xFF,         // LDX $FF

        // absolute
        0xAE, 0x00, 0x10,   // LDX $1000
        0xAE, 0xFF, 0xFF,   // LDX $FFFF
        0xAE, 0x21, 0x00,   // LDX $0021

        // zero page,Y
        0xA0, 0x0F,         // LDY #$0F
        0xB6, 0x10,         // LDX $10,Y    => 0x1F
        0xB6, 0xF8,         // LDX $F8,Y    => 0x07
        0xB6, 0x78,         // LDX $78,Y    => 0x87

        // absolute,Y
        0xA0, 0xF0,         // LDY #$F0
        0xBE, 0x10, 0x0F,   // LDX $0F10,Y    => 0x1000
        0xBE, 0x0F, 0xFF,   // LDX $FF0F,Y    => 0xFFFF
        0xBE, 0x31, 0xFF,   // LDX $FF31,Y    => 0x0021
    };
    init(0, prog, sizeof(prog));
    TOP("LDX #$00")
    TOP("LDX #$01")
    TOP("LDX #$00")
    TOP("LDX #$80")
    TOP("LDX $02")
    TOP("LDX $03")
    TOP("LDX $80")
    TOP("LDX $FF")
    TOP("LDX $1000")
    TOP("LDX $FFFF")
    TOP("LDX $0021")
    TOP("LDY #$0F")
    TOP("LDX $10,Y")
    TOP("LDX $F8,Y")
    TOP("LDX $78,Y")
    TOP("LDY #$F0")
    TOP("LDX $0F10,Y")
    TOP("LDX $FF0F,Y")
    TOP("LDX $FF31,Y")
}

UTEST(w65c02dasm, LDY) {
    uint8_t prog[] = {
        // immediate
        0xA0, 0x00,         // LDY #$00
        0xA0, 0x01,         // LDY #$01
        0xA0, 0x00,         // LDY #$00
        0xA0, 0x80,         // LDY #$80

        // zero page
        0xA4, 0x02,         // LDY $02
        0xA4, 0x03,         // LDY $03
        0xA4, 0x80,         // LDY $80
        0xA4, 0xFF,         // LDY $FF

        // absolute
        0xAC, 0x00, 0x10,   // LDY $1000
        0xAC, 0xFF, 0xFF,   // LDY $FFFF
        0xAC, 0x21, 0x00,   // LDY $0021

        // zero page,X
        0xA2, 0x0F,         // LDX #$0F
        0xB4, 0x10,         // LDY $10,X    => 0x1F
        0xB4, 0xF8,         // LDY $F8,X    => 0x07
        0xB4, 0x78,         // LDY $78,X    => 0x87

        // absolute,X
        0xA2, 0xF0,         // LDX #$F0
        0xBC, 0x10, 0x0F,   // LDY $0F10,X    => 0x1000
        0xBC, 0x0F, 0xFF,   // LDY $FF0F,X    => 0xFFFF
        0xBC, 0x31, 0xFF,   // LDY $FF31,X    => 0x0021
    };
    init(0, prog, sizeof(prog));
    TOP("LDY #$00");
    TOP("LDY #$01");
    TOP("LDY #$00");
    TOP("LDY #$80");
    TOP("LDY $02");
    TOP("LDY $03");
    TOP("LDY $80");
    TOP("LDY $FF");
    TOP("LDY $1000");
    TOP("LDY $FFFF");
    TOP("LDY $0021");
    TOP("LDX #$0F");
    TOP("LDY $10,X");
    TOP("LDY $F8,X");
    TOP("LDY $78,X");
    TOP("LDX #$F0");
    TOP("LDY $0F10,X");
    TOP("LDY $FF0F,X");
    TOP("LDY $FF31,X");
}

UTEST(w65c02dasm, STA_STZ) {
    uint8_t prog[] = {
        0xA9, 0x23,             // LDA #$23
        0xA2, 0x10,             // LDX #$10
        0xA0, 0xC0,             // LDY #$C0
        0x85, 0x10,             // STA $10
        0x8D, 0x34, 0x12,       // STA $1234
        0x95, 0x10,             // STA $10,X
        0x9D, 0x00, 0x20,       // STA $2000,X
        0x99, 0x00, 0x20,       // STA $2000,Y
        0x81, 0x10,             // STA ($10,X)
        0x91, 0x20,             // STA ($20),Y
        0x92, 0x20,             // STA ($20)
        0x64, 0x20,             // STZ $20
        0x74, 0x20,             // STZ $20,X
        0x9C, 0x20, 0x45,       // STZ $4520
        0x9E, 0x20, 0x45,       // STZ $4520,X
    };
    init(0, prog, sizeof(prog));
    TOP("LDA #$23");
    TOP("LDX #$10");
    TOP("LDY #$C0");
    TOP("STA $10");
    TOP("STA $1234");
    TOP("STA $10,X");
    TOP("STA $2000,X");
    TOP("STA $2000,Y");
    TOP("STA ($10,X)");
    TOP("STA ($20),Y");
    TOP("STA ($20)");
    TOP("STZ $20");
    TOP("STZ $20,X");
    TOP("STZ $4520");
    TOP("STZ $4520,X");
}

UTEST(w65c02dasm, STX) {
    uint8_t prog[] = {
        0xA2, 0x23,             // LDX #$23
        0xA0, 0x10,             // LDY #$10
        0x86, 0x10,             // STX $10
        0x8E, 0x34, 0x12,       // STX $1234
        0x96, 0x10,             // STX $10,Y
    };
    init(0, prog, sizeof(prog));
    TOP("LDX #$23");
    TOP("LDY #$10");
    TOP("STX $10");
    TOP("STX $1234");
    TOP("STX $10,Y");
}

UTEST(w65c02dasm, STY) {
    uint8_t prog[] = {
        0xA0, 0x23,             // LDY #$23
        0xA2, 0x10,             // LDX #$10
        0x84, 0x10,             // STY $10
        0x8C, 0x34, 0x12,       // STY $1234
        0x94, 0x10,             // STY $10,X
    };
    init(0, prog, sizeof(prog));
    TOP("LDY #$23");
    TOP("LDX #$10");
    TOP("STY $10");
    TOP("STY $1234");
    TOP("STY $10,X");
}

UTEST(w65c02dasm, TAX_TXA) {
    uint8_t prog[] = {
        0xA9, 0x00,     // LDA #$00
        0xA2, 0x10,     // LDX #$10
        0xAA,           // TAX
        0xA9, 0xF0,     // LDA #$F0
        0x8A,           // TXA
        0xA9, 0xF0,     // LDA #$F0
        0xA2, 0x00,     // LDX #$00
        0xAA,           // TAX
        0xA9, 0x01,     // LDA #$01
        0x8A,           // TXA
    };
    init(0, prog, sizeof(prog));
    TOP("LDA #$00");
    TOP("LDX #$10");
    TOP("TAX");
    TOP("LDA #$F0");
    TOP("TXA");
    TOP("LDA #$F0");
    TOP("LDX #$00");
    TOP("TAX");
    TOP("LDA #$01");
    TOP("TXA");
}

UTEST(w65c02dasm, TAY_TYA) {
    uint8_t prog[] = {
        0xA9, 0x00,     // LDA #$00
        0xA0, 0x10,     // LDY #$10
        0xA8,           // TAY
        0xA9, 0xF0,     // LDA #$F0
        0x98,           // TYA
        0xA9, 0xF0,     // LDA #$F0
        0xA0, 0x00,     // LDY #$00
        0xA8,           // TAY
        0xA9, 0x01,     // LDA #$01
        0x98,           // TYA
    };
    init(0, prog, sizeof(prog));
    TOP("LDA #$00");
    TOP("LDY #$10");
    TOP("TAY");
    TOP("LDA #$F0");
    TOP("TYA");
    TOP("LDA #$F0");
    TOP("LDY #$00");
    TOP("TAY");
    TOP("LDA #$01");
    TOP("TYA");
}

UTEST(w65c02dasm, DEX_INX_DEY_INY) {
    uint8_t prog[] = {
        0xA2, 0x01,     // LDX #$01
        0xCA,           // DEX
        0xCA,           // DEX
        0xE8,           // INX
        0xE8,           // INX
        0xA0, 0x01,     // LDY #$01
        0x88,           // DEY
        0x88,           // DEY
        0xC8,           // INY
        0xC8,           // INY
    };
    init(0, prog, sizeof(prog));
    TOP("LDX #$01");
    TOP("DEX");
    TOP("DEX");
    TOP("INX");
    TOP("INX");
    TOP("LDY #$01");
    TOP("DEY");
    TOP("DEY");
    TOP("INY");
    TOP("INY");
}

UTEST(w65c02dasm, TXS_TSX) {
    uint8_t prog[] = {
        0xA2, 0xAA,     // LDX #$AA
        0xA9, 0x00,     // LDA #$00
        0x9A,           // TXS
        0xAA,           // TAX
        0xBA,           // TSX
    };
    init(0, prog, sizeof(prog));
    TOP("LDX #$AA");
    TOP("LDA #$00");
    TOP("TXS");
    TOP("TAX");
    TOP("TSX");
}

UTEST(w65c02dasm, ORA) {
    uint8_t prog[] = {
        0xA9, 0x00,         // LDA #$00
        0xA2, 0x01,         // LDX #$01
        0xA0, 0x02,         // LDY #$02
        0x09, 0x00,         // ORA #$00
        0x05, 0x10,         // ORA $10
        0x15, 0x10,         // ORA $10,X
        0x0D, 0x00, 0x10,   // ORA $1000
        0x1D, 0x00, 0x10,   // ORA $1000,X
        0x19, 0x00, 0x10,   // ORA $1000,Y
        0x01, 0x22,         // ORA ($22,X)
        0x11, 0x20,         // ORA ($20),Y
        0x12, 0x20,         // ORA ($20)
    };
    init(0, prog, sizeof(prog));
    TOP("LDA #$00");
    TOP("LDX #$01");
    TOP("LDY #$02");
    TOP("ORA #$00");
    TOP("ORA $10");
    TOP("ORA $10,X");
    TOP("ORA $1000");
    TOP("ORA $1000,X");
    TOP("ORA $1000,Y");
    TOP("ORA ($22,X)");
    TOP("ORA ($20),Y");
    TOP("ORA ($20)");
}

UTEST(w65c02dasm, AND) {
    uint8_t prog[] = {
        0xA9, 0xFF,         // LDA #$FF
        0xA2, 0x01,         // LDX #$01
        0xA0, 0x02,         // LDY #$02
        0x29, 0xFF,         // AND #$FF
        0x25, 0x10,         // AND $10
        0x35, 0x10,         // AND $10,X
        0x2d, 0x00, 0x10,   // AND $1000
        0x3d, 0x00, 0x10,   // AND $1000,X
        0x39, 0x00, 0x10,   // AND $1000,Y
        0x21, 0x22,         // AND ($22,X)
        0x31, 0x20,         // AND ($20),Y
        0x32, 0x20,         // AND ($20)
    };
    init(0, prog, sizeof(prog));
    TOP("LDA #$FF");
    TOP("LDX #$01");
    TOP("LDY #$02");
    TOP("AND #$FF");
    TOP("AND $10");
    TOP("AND $10,X");
    TOP("AND $1000");
    TOP("AND $1000,X");
    TOP("AND $1000,Y");
    TOP("AND ($22,X)");
    TOP("AND ($20),Y");
    TOP("AND ($20)");
}

UTEST(w65c02dasm, EOR) {
    uint8_t prog[] = {
        0xA9, 0xFF,         // LDA #$FF
        0xA2, 0x01,         // LDX #$01
        0xA0, 0x02,         // LDY #$02
        0x49, 0xFF,         // EOR #$FF
        0x45, 0x10,         // EOR $10
        0x55, 0x10,         // EOR $10,X
        0x4d, 0x00, 0x10,   // EOR $1000
        0x5d, 0x00, 0x10,   // EOR $1000,X
        0x59, 0x00, 0x10,   // EOR $1000,Y
        0x41, 0x22,         // EOR ($22,X)
        0x51, 0x20,         // EOR ($20),Y
        0x52, 0x20,         // EOR ($20)
    };
    init(0, prog, sizeof(prog));
    TOP("LDA #$FF");
    TOP("LDX #$01");
    TOP("LDY #$02");
    TOP("EOR #$FF");
    TOP("EOR $10");
    TOP("EOR $10,X");
    TOP("EOR $1000");
    TOP("EOR $1000,X");
    TOP("EOR $1000,Y");
    TOP("EOR ($22,X)");
    TOP("EOR ($20),Y");
    TOP("EOR ($20)");
}

UTEST(w65c02dasm, NOP) {
    uint8_t prog[] = {
        0xEA,        // NOP
    };
    init(0, prog, sizeof(prog));
    TOP("NOP");
}

UTEST(w65c02dasm, PHA_PLA_PHP_PLP_PHY_PLY_PHX) {
    uint8_t prog[] = {
        0xA9, 0x23,     // LDA #$23
        0x48,           // PHA
        0xA9, 0x32,     // LDA #$32
        0x68,           // PLA
        0x08,           // PHP
        0xA9, 0x00,     // LDA #$00
        0x28,           // PLP
        0x5A,           // PHY
        0x7A,           // PLY
        0xDA,           // PHX
        0xFA,           // PLX
    };
    init(0, prog, sizeof(prog));
    TOP("LDA #$23");
    TOP("PHA");
    TOP("LDA #$32");
    TOP("PLA");
    TOP("PHP");
    TOP("LDA #$00");
    TOP("PLP");
    TOP("PHY");
    TOP("PLY");
    TOP("PHX");
    TOP("PLX");
}

UTEST(w65c02dasm, CLC_SEC_CLI_SEI_CLV_CLD_SED) {
    uint8_t prog[] = {
        0xB8,       // CLV
        0x78,       // SEI
        0x58,       // CLI
        0x38,       // SEC
        0x18,       // CLC
        0xF8,       // SED
        0xD8,       // CLD
    };
    init(0, prog, sizeof(prog));
    TOP("CLV");
    TOP("SEI");
    TOP("CLI");
    TOP("SEC");
    TOP("CLC");
    TOP("SED");
    TOP("CLD");
}

UTEST(w65c02dasm, INC_DEC) {
    uint8_t prog[] = {
        0xA2, 0x10,         // LDX #$10
        0xE6, 0x33,         // INC $33
        0xF6, 0x33,         // INC $33,X
        0xEE, 0x00, 0x10,   // INC $1000
        0xFE, 0x00, 0x10,   // INC $1000,X
        0x1A,               // INC A / INA
        0xC6, 0x33,         // DEC $33
        0xD6, 0x33,         // DEC $33,X
        0xCE, 0x00, 0x10,   // DEC $1000
        0xDE, 0x00, 0x10,   // DEC $1000,X
        0x3A,               // DEC A / DEA
    };
    init(0, prog, sizeof(prog));
    TOP("LDX #$10");
    TOP("INC $33");
    TOP("INC $33,X");
    TOP("INC $1000");
    TOP("INC $1000,X");
    TOP("INA");
    TOP("DEC $33");
    TOP("DEC $33,X");
    TOP("DEC $1000");
    TOP("DEC $1000,X");
    TOP("DEA");
}

UTEST(w65c02dasm, ADC_SBC) {
    uint8_t prog[] = {
        0xA9, 0x01,         // LDA #$01
        0x85, 0x10,         // STA $10
        0x8D, 0x00, 0x10,   // STA $1000
        0xA9, 0xFC,         // LDA #$FC
        0xA2, 0x08,         // LDX #$08
        0xA0, 0x04,         // LDY #$04
        0x18,               // CLC
        0x69, 0x01,         // ADC #$01
        0x65, 0x10,         // ADC $10
        0x75, 0x08,         // ADC $8,X
        0x6D, 0x00, 0x10,   // ADC $1000
        0x7D, 0xF8, 0x0F,   // ADC $0FF8,X
        0x79, 0xFC, 0x0F,   // ADC $0FFC,Y
        0x61, 0xFC,         // ADC ($FC,X)
        0x71, 0xFC,         // ADC ($FC,Y)
        0x72, 0xFC,         // ADC ($FC)
        0xF9, 0xFC, 0x0F,   // SBC $0FFC,Y
        0xFD, 0xF8, 0x0F,   // SBC $0FF8,X
        0xED, 0x00, 0x10,   // SBC $1000
        0xF5, 0x08,         // SBC $8,X
        0xE5, 0x10,         // SBC $10
        0xE9, 0x01,         // SBC #$01
        0xE1, 0xFC,         // SBC ($FC,X)
        0xF1, 0xFC,         // SBC ($FC),Y
        0xF2, 0xFC,         // SBC ($FC)
    };
    init(0, prog, sizeof(prog));
    TOP("LDA #$01");
    TOP("STA $10");
    TOP("STA $1000");
    TOP("LDA #$FC");
    TOP("LDX #$08");
    TOP("LDY #$04");
    TOP("CLC");
    TOP("ADC #$01");
    TOP("ADC $10");
    TOP("ADC $08,X");
    TOP("ADC $1000");
    TOP("ADC $0FF8,X");
    TOP("ADC $0FFC,Y");
    TOP("ADC ($FC,X)");
    TOP("ADC ($FC),Y");
    TOP("ADC ($FC)");
    TOP("SBC $0FFC,Y");
    TOP("SBC $0FF8,X");
    TOP("SBC $1000");
    TOP("SBC $08,X");
    TOP("SBC $10");
    TOP("SBC #$01");
    TOP("SBC ($FC,X)");
    TOP("SBC ($FC),Y");
    TOP("SBC ($FC)");
}

UTEST(w65c02dasm, CMP_CPX_CPY) {
    uint8_t prog[] = {
        0xA9, 0x01,       // LDA #$01
        0xA2, 0x02,       // LDX #$02
        0xA0, 0x03,       // LDY #$03
        0xC9, 0x00,       // CMP #$00
        0xC9, 0x01,       // CMP #$01
        0xC5, 0x02,       // CMP $02
        0xC1, 0x02,       // CMP ($02,X)
        0xCD, 0x03, 0x12, // CMP $1203
        0xD1, 0x02,       // CMP ($02),Y
        0xD2, 0x02,       // CMP ($02)
        0xD5, 0x02,       // CMP $02,X
        0xD9, 0x03, 0x12, // CMP $1203,Y
        0xDD, 0x03, 0x12, // CMP $1203,X
        0xE0, 0x01,       // CPX #$01
        0xE4, 0x02,       // CPX $02
        0xEC, 0x03, 0x12, // CPX $1203
        0xC0, 0x02,       // CPY #$02
        0xC4, 0x02,       // CPY $02
        0xC0, 0x03,       // CPY #$03
        0xCC, 0x03, 0x12, // CPY $1203
    };
    init(0, prog, sizeof(prog));
    TOP("LDA #$01");
    TOP("LDX #$02");
    TOP("LDY #$03");
    TOP("CMP #$00");
    TOP("CMP #$01");
    TOP("CMP $02");
    TOP("CMP ($02,X)");
    TOP("CMP $1203");
    TOP("CMP ($02),Y");
    TOP("CMP ($02)");
    TOP("CMP $02,X");
    TOP("CMP $1203,Y");
    TOP("CMP $1203,X");
    TOP("CPX #$01");
    TOP("CPX $02");
    TOP("CPX $1203");
    TOP("CPY #$02");
    TOP("CPY $02");
    TOP("CPY #$03");
    TOP("CPY $1203");
}

UTEST(w65c02dasm, ASL) {
    uint8_t prog[] = {
        0xA9, 0x81,       // LDA #$81
        0xA2, 0x01,       // LDX #$01
        0x85, 0x10,       // STA $10
        0x06, 0x10,       // ASL $10
        0x16, 0x0F,       // ASL $0F,X
        0x0A,             // ASL
        0x0E, 0x12, 0x34, // ASL $3412
        0x1E, 0x12, 0x34  // ASL $3412,X
    };
    init(0, prog, sizeof(prog));
    TOP("LDA #$81");
    TOP("LDX #$01");
    TOP("STA $10");
    TOP("ASL $10");
    TOP("ASL $0F,X");
    TOP("ASL");
    TOP("ASL $3412");
    TOP("ASL $3412,X");
}

UTEST(w65c02dasm, LSR) {
    uint8_t prog[] = {
        0xA9, 0x81,       // LDA #$81
        0xA2, 0x01,       // LDX #$01
        0x85, 0x10,       // STA $10
        0x46, 0x10,       // LSR $10
        0x56, 0x0F,       // LSR $0F,X
        0x4A,             // LSR
        0x4E, 0x33, 0x44, // LSR $4433
        0x5E, 0x33, 0x44  // LSR $4433,X
    };
    init(0, prog, sizeof(prog));
    TOP("LDA #$81");
    TOP("LDX #$01");
    TOP("STA $10");
    TOP("LSR $10");
    TOP("LSR $0F,X");
    TOP("LSR");
    TOP("LSR $4433");
    TOP("LSR $4433,X");
}

UTEST(w65c02dasm, ROR_ROL) {
    uint8_t prog[] = {
        0xA9, 0x81,       // LDA #$81
        0xA2, 0x01,       // LDX #$01
        0x85, 0x10,       // STA $10
        0x26, 0x10,       // ROL $10
        0x36, 0x0F,       // ROL $0F,X
        0x76, 0x0F,       // ROR $0F,X
        0x66, 0x10,       // ROR $10
        0x6A,             // ROR
        0x6E, 0x11, 0x22, // ROR $2211
        0x7E, 0x11, 0x22, // ROR $2211,X
        0x2A,             // ROL
        0x2E, 0x11, 0x22, // ROL $2211
        0x3E, 0x11, 0x22, // ROL $2211,X
    };
    init(0, prog, sizeof(prog));
    TOP("LDA #$81");
    TOP("LDX #$01");
    TOP("STA $10");
    TOP("ROL $10");
    TOP("ROL $0F,X");
    TOP("ROR $0F,X");
    TOP("ROR $10");
    TOP("ROR");
    TOP("ROR $2211");
    TOP("ROR $2211,X");
    TOP("ROL");
    TOP("ROL $2211");
    TOP("ROL $2211,X");
}

UTEST(w65c02dasm, BIT) {
    uint8_t prog[] = {
        0xA9, 0x00,         // LDA #$00
        0x85, 0x1F,         // STA $1F
        0xA9, 0x80,         // LDA #$80
        0x85, 0x20,         // STA $20
        0xA9, 0xC0,         // LDA #$C0
        0x8D, 0x00, 0x10,   // STA $1000
        0x89, 0x1F,         // BIT #$1F
        0x24, 0x1F,         // BIT $1F
        0x24, 0x20,         // BIT $20
        0x34, 0x1F,         // BIT $1F,X
        0x2C, 0x00, 0x10,   // BIT $1000
        0x3C, 0x00, 0x10    // BIT $1000,X
    };
    init(0, prog, sizeof(prog));
    TOP("LDA #$00");
    TOP("STA $1F");
    TOP("LDA #$80");
    TOP("STA $20");
    TOP("LDA #$C0");
    TOP("STA $1000");
    TOP("BIT #$1F");
    TOP("BIT $1F");
    TOP("BIT $20");
    TOP("BIT $1F,X");
    TOP("BIT $1000");
    TOP("BIT $1000,X");
}

UTEST(w65c02dasm, BNE_BEQ_BPL_BMI_BVC_BVS_BRA_BCC_BCS) {
    uint8_t prog[] = {
        0xA9, 0x10,         // LDA #$10
        0xC9, 0x10,         // CMP #$10
        0xF0, 0x02,         // BEQ eq
        0xA9, 0x0F,         // ne: LDA #$0F
        0xC9, 0x0F,         // eq: CMP #$0F
        0xD0, 0xFA,         // BNE ne -> executed 2x, second time not taken
        0xEA,
        0x10, 0xED,         // BPL $01FC
        0x30, 0xED,         // BMI $01FE
        0x50, 0xED,         // BVC $0201
        0x70, 0xED,         // BVS $0202
        0x80, 0xED,         // BRA $0204
        0x90, 0xED,         // BCC $0205
        0xB0, 0xED,         // BCS $0208
    };
    init(0x0200, prog, sizeof(prog));
    TOP("LDA #$10");
    TOP("CMP #$10");
    TOP("BEQ $0208");
    TOP("LDA #$0F");
    TOP("CMP #$0F");
    TOP("BNE $0206");
    TOP("NOP");
    TOP("BPL $01FC");
    TOP("BMI $01FE");
    TOP("BVC $0200");
    TOP("BVS $0202");
    TOP("BRA $0204");
    TOP("BCC $0206");
    TOP("BCS $0208");

}

UTEST(w65c02dasm, JMP) {
    uint8_t prog[] = {
        0x4C, 0x00, 0x10,   // JMP $1000
        0x7C, 0x00, 0x10,   // JMP ($1000,X)
    };
    init(0x0200, prog, sizeof(prog));
    TOP("JMP $1000");
    TOP("JMP ($1000,X)");
}

UTEST(w65c02dasm, JMP_indirect_samepage) {
    uint8_t prog[] = {
        0xA9, 0x33,         // LDA #$33
        0x8D, 0x10, 0x21,   // STA $2110
        0xA9, 0x22,         // LDA #$22
        0x8D, 0x11, 0x21,   // STA $2111
        0x6C, 0x10, 0x21,   // JMP ($2110)
    };
    init(0x0200, prog, sizeof(prog));
    TOP("LDA #$33");
    TOP("STA $2110");
    TOP("LDA #$22");
    TOP("STA $2111");
    TOP("JMP ($2110)");
}

UTEST(w65c02dasm, JMP_indirect_wrap) {
    uint8_t prog[] = {
        0xA9, 0x33,         // LDA #$33
        0x8D, 0xFF, 0x21,   // STA $21FF
        0xA9, 0x22,         // LDA #$22
        0x8D, 0x00, 0x21,   // STA $2100    // note: wraps around!
        0x6C, 0xFF, 0x21,   // JMP ($21FF)
    };
    init(0x0200, prog, sizeof(prog));
    TOP("LDA #$33");
    TOP("STA $21FF");
    TOP("LDA #$22");
    TOP("STA $2100");
    TOP("JMP ($21FF)");
}

UTEST(w65c02dasm, JSR_RTS) {
    uint8_t prog[] = {
        0x20, 0x05, 0x03,   // JSR fun
        0xEA, 0xEA,         // NOP, NOP
        0xEA,               // fun: NOP
        0x60,               // RTS
    };
    init(0x0300, prog, sizeof(prog));
    TOP("JSR $0305");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("RTS");
}

UTEST(w65c02dasm, RTI) {
    uint8_t prog[] = {
        0xA9, 0x11,     // LDA #$11
        0x48,           // PHA
        0xA9, 0x22,     // LDA #$22
        0x48,           // PHA
        0xA9, 0x33,     // LDA #$33
        0x48,           // PHA
        0x40,           // RTI
    };
    init(0x0200, prog, sizeof(prog));
    TOP("LDA #$11");
    TOP("PHA");
    TOP("LDA #$22");
    TOP("PHA");
    TOP("LDA #$33");
    TOP("PHA");
    TOP("RTI");
}

UTEST(w65c02dasm, TSB_TRB) {
    uint8_t prog[] = {
        0x04, 0x11,       // TSB $11
        0x0C, 0x11, 0x22, // TSB $2211
        0x14, 0x11,       // TRB $11
        0x1C, 0x11, 0x22, // TRB $2211,
    };
    init(0x0200, prog, sizeof(prog));
    TOP("TSB $11");
    TOP("TSB $2211");
    TOP("TRB $11");
    TOP("TRB $2211");
}

UTEST(w65c02dasm, RMB_SMB) {
    uint8_t prog[] = {
        0x07, 0x11,       // RMB0 $11
        0x17, 0x11,       // RMB1 $11
        0x27, 0x11,       // RMB2 $11
        0x37, 0x11,       // RMB3 $11
        0x47, 0x11,       // RMB4 $11
        0x57, 0x11,       // RMB5 $11
        0x67, 0x11,       // RMB6 $11
        0x77, 0x11,       // RMB7 $11
        0x87, 0x11,       // SMB0 $11
        0x97, 0x11,       // SMB1 $11
        0xA7, 0x11,       // SMB2 $11
        0xB7, 0x11,       // SMB3 $11
        0xC7, 0x11,       // SMB4 $11
        0xD7, 0x11,       // SMB5 $11
        0xE7, 0x11,       // SMB6 $11
        0xF7, 0x11,       // SMB7 $11
    };
    init(0x0200, prog, sizeof(prog));
    TOP("RMB0 $11");
    TOP("RMB1 $11");
    TOP("RMB2 $11");
    TOP("RMB3 $11");
    TOP("RMB4 $11");
    TOP("RMB5 $11");
    TOP("RMB6 $11");
    TOP("RMB7 $11");
    TOP("SMB0 $11");
    TOP("SMB1 $11");
    TOP("SMB2 $11");
    TOP("SMB3 $11");
    TOP("SMB4 $11");
    TOP("SMB5 $11");
    TOP("SMB6 $11");
    TOP("SMB7 $11");
}

UTEST(w65c02dasm, BBR_BBS) {
    uint8_t prog[] = {
        0x0F, 0x11, 0xED,   // BBR0 $11,$01F0
        0x1F, 0x11, 0xED,   // BBR1 $11,$01F3
        0x2F, 0x11, 0xED,   // BBR2 $11,$01F6
        0x3F, 0x11, 0xED,   // BBR3 $11,$01F9
        0x4F, 0x11, 0xED,   // BBR4 $11,$01FC
        0x5F, 0x11, 0xED,   // BBR5 $11,$01FF
        0x6F, 0x11, 0xED,   // BBR6 $11,$0202
        0x7F, 0x11, 0xED,   // BBR7 $11,$0205
        0x8F, 0x11, 0xED,   // BBS0 $11,$0208
        0x9F, 0x11, 0xED,   // BBS1 $11,$020B
        0xAF, 0x11, 0xED,   // BBS2 $11,$020E
        0xBF, 0x11, 0xED,   // BBS3 $11,$0211
        0xCF, 0x11, 0xED,   // BBS4 $11,$0214
        0xDF, 0x11, 0xED,   // BBS5 $11,$0217
        0xEF, 0x11, 0xED,   // BBS6 $11,$021A
        0xFF, 0x11, 0xED,   // BBS7 $11,$021D
    };
    init(0x0200, prog, sizeof(prog));
    TOP("BBR0 $11,$01F0");
    TOP("BBR1 $11,$01F3");
    TOP("BBR2 $11,$01F6");
    TOP("BBR3 $11,$01F9");
    TOP("BBR4 $11,$01FC");
    TOP("BBR5 $11,$01FF");
    TOP("BBR6 $11,$0202");
    TOP("BBR7 $11,$0205");
    TOP("BBS0 $11,$0208");
    TOP("BBS1 $11,$020B");
    TOP("BBS2 $11,$020E");
    TOP("BBS3 $11,$0211");
    TOP("BBS4 $11,$0214");
    TOP("BBS5 $11,$0217");
    TOP("BBS6 $11,$021A");
    TOP("BBS7 $11,$021D");
}

UTEST(w65c02dasm, BRK_WAI_STP) {
    uint8_t prog[] = {
        0x00,  // BRK
        0xCB,  // WAI
        0xDB,  // STP
    };
    init(0x0200, prog, sizeof(prog));
    TOP("BRK");
    TOP("WAI");
    TOP("STP");
}

UTEST(w65c02dasm, NOP_undef) {
    // Undefined instructions treated as NOPs.
    // Note that some opcodes implies 1 or 2 extra dummy reads
    uint8_t prog[] = {
        0x02, 0xEE,
        0x03,
        0x0B,
        0x13,
        0x1B,
        0x22, 0xEE,
        0x23,
        0x2B,
        0x33,
        0x3B,
        0x42, 0xEE,
        0x43,
        0x44, 0xEE,
        0x4B,
        0x53,
        0x54, 0xEE,
        0x5B,
        0x5C, 0xEE, 0xFF,
        0x62, 0xEE,
        0x63,
        0x6B,
        0x73,
        0x7B,
        0x82, 0xEE,
        0x83,
        0x8B,
        0x93,
        0x9B,
        0xA3,
        0xAB,
        0xB3,
        0xBB,
        0xC2, 0xEE,
        0xC3,
        0xD3,
        0xD4, 0xEE,
        0xDC, 0xEE, 0xFF,
        0xE2, 0xEE,
        0xE3,
        0xEB,
        0xF3,
        0xF4, 0xEE,
        0xFB,
        0xFC,
    };
    init(0, prog, sizeof(prog));
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
    TOP("NOP");
}
