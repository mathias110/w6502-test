#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "chips/w65c02.h"
#include "util/w65c02dasm.h"

#include "real_w65c02.h"
#include "utest.h"

static uint8_t memory[1 << 16];

static uint8_t w65c02dasm_input(void *user_data) {
  uint8_t **ptr = (uint8_t **)user_data;
  uint8_t res = (*ptr)[0];
  (*ptr)++;
  return res;
}

static void w65c02dasm_output(char c, void *user_data) {
  (void)user_data;
  printf("%c", c);
}

static void load(ProgramFragment fragments[]) {
  memset(memory, 0, sizeof(memory));
  for (int i = 0; fragments[i].count != 0; i++) {
    memcpy(&memory[fragments[i].baseAddress], fragments[i].data,
           fragments[i].count);
  }
}

static uint64_t stepOne(w65c02_t *cpu, uint64_t pins) {
  pins = w65c02_tick(cpu, pins);
  const uint16_t addr = W65C02_GET_ADDR(pins);
  // perform memory access
  if (pins & W65C02_RW) {
    // a memory read
    W65C02_SET_DATA(pins, memory[addr]);
  } else {
    // a memory write
    memory[addr] = W65C02_GET_DATA(pins);
  }
  return pins;
}

static void trace(size_t cycle, w65c02_t *cpu, uint64_t pins) {
  char pstr[] = "NV1BDIZC";
  uint8_t p = w65c02_p(cpu);
  for (int i = 0; i < 8; i++) {
    if ((p & (1 << (7 - i))) == 0)
      pstr[i] = '_';
  }
  uint16_t addr = W65C02_GET_ADDR(pins);
  printf("%5ld : %016lx %c %c %c %c %c %c addr=%04x data=%02x [pc:%04x a:%02x "
         "x:%02x y:%02x p:%02x %s %04x] ",
         cycle, pins, (pins & W65C02_VP) ? 'V' : ' ',
         (pins & W65C02_RDY) ? 'D' : ' ', (pins & W65C02_NMI) ? 'N' : ' ',
         (pins & W65C02_IRQ) ? 'I' : ' ', (pins & W65C02_SYNC) ? 'S' : ' ',
         (pins & W65C02_RW) ? 'R' : 'W', addr, W65C02_GET_DATA(pins),
         w65c02_pc(cpu), w65c02_a(cpu), w65c02_x(cpu), w65c02_y(cpu), p, pstr,
         cpu->AD);
  if (pins & W65C02_SYNC) {
    uint8_t *memptr = &memory[addr];
    w65c02dasm_op(addr, w65c02dasm_input, w65c02dasm_output, &memptr);
  }
  printf("\n");
}

static int trace_run = 0;
static int drive_rdy = 0;
static bool run_program(const PinState pin_states[], size_t cycles) {
  w65c02_t cpu;
  uint64_t pins = w65c02_init(&cpu, &(w65c02_desc_t){.bcd_disabled = false});

  for (int k = 0; k < 6; k++) {
    pins = stepOne(&cpu, pins);
    if (trace_run) {
      trace(k, &cpu, pins);
    }
  }

  for (size_t k = 0; k < cycles; k++) {
    const PinState *s = &pin_states[k];
    pins = stepOne(&cpu, pins);
    if (s->irq == 0) {
      pins |= W65C02_IRQ;
    } else {
      pins &= ~W65C02_IRQ;
    }
    if (s->nmi == 0) {
      pins |= W65C02_NMI;
    } else {
      pins &= ~W65C02_NMI;
    }
    if (drive_rdy) {
      if (s->rdy == 0) {
        pins |= W65C02_RDY;
      } else {
        pins &= ~W65C02_RDY;
      }
    }

    if (trace_run) {
      trace(k, &cpu, pins);
    }

    const bool sync = (pins & W65C02_SYNC) != 0;
    const bool rdy = (pins & W65C02_RDY) != 0;
    const bool rw = (pins & W65C02_RW) != 0;
    const bool vpb = (pins & W65C02_VP) != 0;
    const uint16_t addr = W65C02_GET_ADDR(pins);
    const uint8_t data = W65C02_GET_DATA(pins);
    // if (sync) {
    //   printf("OP: %02x\n", data);
    // }
    if (sync != s->sync) {
      printf("SYNC error: actual %d expected %d\n", sync, s->sync);
      return false;
    }

    if (rdy == s->rdy) {
      printf("RDY error: actual %d expected %d\n", rdy, 1 - s->rdy);
      return false;
    }

    if (rw != s->rw) {
      printf("RW error: actual %d expected %d\n", rw, s->rw);
      return false;
    }

    if (vpb == s->vpb) { // active low on real chip
      printf("VPB error: actual %d expected %d\n", vpb, 1 - s->vpb);
      return false;
    }

    if (addr != s->addr) {
      printf("Address error: actual %04x expected %04x\n", addr, s->addr);
      return false;
    }

    if (data != s->data) {
      printf("Data error: actual %02x expected %02x\n", data, s->data);
      return false;
    }
  }
  return true;
}

struct real_w65c02 {};

UTEST_F_SETUP(real_w65c02) {
  (void)utest_result;
  (void)utest_fixture;
  trace_run = 0;
  drive_rdy = 0;
}

UTEST_F_TEARDOWN(real_w65c02) {
  (void)utest_result;
  (void)utest_fixture;
}

#define SINGLE(tc)                                                             \
  UTEST_F(real_w65c02, tc) {                                                   \
    extern ProgramFragment tc##_prog[];                                        \
    extern PinState tc##_pin[];                                                \
    extern size_t tc##_cycles;                                                 \
    (void)utest_fixture;                                                       \
    bool result;                                                               \
    load(tc##_prog);                                                           \
    result = run_program(tc##_pin, tc##_cycles);                               \
    EXPECT_TRUE(result);                                                       \
  }

#define SINGLE_T(tc)                                                           \
  UTEST_F(real_w65c02, tc) {                                                   \
    extern ProgramFragment tc##_prog[];                                        \
    extern PinState tc##_pin[];                                                \
    extern size_t tc##_cycles;                                                 \
    bool result;                                                               \
    (void)utest_fixture;                                                       \
    load(tc##_prog);                                                           \
    trace_run = 1;                                                             \
    result = run_program(tc##_pin, tc##_cycles);                               \
    EXPECT_TRUE(result);                                                       \
  }

#define SUITE(tc)                                                              \
  UTEST_F(real_w65c02, tc) {                                                   \
    extern ProgramFragment tc##_prog[];                                        \
    extern PinTestSuite tc##_suite[];                                          \
    bool result = true;                                                        \
    (void)utest_fixture;                                                       \
    for (int i = 0; tc##_suite[i].pinstate != 0 && result; i++) {              \
      load(tc##_prog);                                                         \
      result = run_program(tc##_suite[i].pinstate, tc##_suite[i].cycles);      \
      EXPECT_TRUE(result);                                                     \
    }                                                                          \
  }

#define SUITE_T(tc)                                                            \
  UTEST_F(real_w65c02, tc) {                                                   \
    extern ProgramFragment tc##_prog[];                                        \
    extern PinTestSuite tc##_suite[];                                          \
    (void)utest_fixture;                                                       \
    bool result = true;                                                        \
    trace_run = 1;                                                             \
    for (int i = 0; tc##_suite[i].pinstate != 0 && result; i++) {              \
      printf("Running subtest %d\n", i);                                       \
      load(tc##_prog);                                                         \
      result = run_program(tc##_suite[i].pinstate, tc##_suite[i].cycles);      \
      EXPECT_TRUE(result);                                                     \
    }                                                                          \
  }

SINGLE(adc);
SINGLE(and);
SINGLE(asl);
SINGLE(bbr);
SINGLE(bbs);
SINGLE(bcc);
SINGLE(bcs);
SINGLE(beq);
SINGLE(bit);
SINGLE(bmi);
SINGLE(bne);
SINGLE(bpl);
SINGLE(bra);
SINGLE(brk);
SINGLE(bvc);
SINGLE(bvs);
SINGLE(cmp);
SINGLE(dec);
SINGLE(eor);
SINGLE(inc);
SINGLE(jmp);
SINGLE(jsr);
SINGLE(load);
SINGLE(lsr);
SINGLE(nop_undef);
SINGLE(ora);
SINGLE(stack);
SINGLE(rmb_smb);
SINGLE(rol);
SINGLE(ror);
SINGLE(sbc);
SINGLE(store);
SINGLE(stp);
SINGLE(xfer);
SINGLE(trb_tsb);
SINGLE(set_clear_pbits);
SUITE(wai);
SUITE(wai_int_en);
SUITE(irq);
SUITE(irq_branch);
SUITE(nmi);
SUITE(nmi_branch);

UTEST_F(real_w65c02, rdy) {
  extern ProgramFragment rdy_prog[];
  extern PinTestSuite rdy_suite[];
  bool result = true;
  (void)utest_fixture;
  trace_run = 0;
  drive_rdy = 1;
  load(rdy_prog);
  for (int i = 0; rdy_suite[i].pinstate != 0 && result; i++) {
    if (trace_run) {
      printf("Running subtest %d\n", i);
    }
    result = run_program(rdy_suite[i].pinstate, rdy_suite[i].cycles);
    EXPECT_TRUE(result);
  }
}
