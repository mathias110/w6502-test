#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "chips/w65c02.h"
#include "util/w65c02dasm.h"

#include "utest.h"

struct w65c02_test {
  w65c02_t cpu;
  uint64_t pins;
  uint8_t memory[1 << 16];
};

static void w8(struct w65c02_test *fixture, uint16_t addr, uint8_t data) {
  fixture->memory[addr] = data;
}

static void w16(struct w65c02_test *fixture, uint16_t addr, uint16_t data) {
  fixture->memory[addr] = data & 0xff;
  fixture->memory[addr + 1] = (data >> 8) & 0xff;
}

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

static int trace_run = 1;
static void trace(struct w65c02_test *fixture) {
  char pstr[] = "NV1BDIZC";
  const uint8_t p = w65c02_p(&fixture->cpu);
  for (int i = 0; i < 8; i++) {
    if ((p & (1 << (7 - i))) == 0)
      pstr[i] = '_';
  }
  const uint64_t pins = fixture->pins;
  w65c02_t *cpu = &fixture->cpu;
  const uint16_t addr = W65C02_GET_ADDR(pins);
  printf("%016lx %c %c %c %c %c %c addr=%04x data=%02x [pc:%04x a:%02x "
         "x:%02x y:%02x p:%02x %s] ",
         pins, (pins & W65C02_VP) ? 'V' : ' ', (pins & W65C02_RDY) ? 'D' : ' ',
         (pins & W65C02_NMI) ? 'N' : ' ', (pins & W65C02_IRQ) ? 'I' : ' ',
         (pins & W65C02_SYNC) ? 'S' : ' ', (pins & W65C02_RW) ? 'R' : 'W', addr,
         W65C02_GET_DATA(pins), w65c02_pc(cpu), w65c02_a(cpu), w65c02_x(cpu),
         w65c02_y(cpu), p, pstr);
  if (pins & W65C02_SYNC) {
    uint8_t *memptr = &(fixture->memory[addr]);
    w65c02dasm_op(addr, w65c02dasm_input, w65c02dasm_output, &memptr);
  }
  printf("\n");
}

static void stepOne(struct w65c02_test *fixture) {
  uint64_t pins = fixture->pins;
  pins = w65c02_tick(&fixture->cpu, pins);
  const uint16_t addr = W65C02_GET_ADDR(pins);
  // perform memory access
  if (pins & W65C02_RW) {
    // a memory read
    W65C02_SET_DATA(pins, fixture->memory[addr]);
  } else {
    // a memory write
    fixture->memory[addr] = W65C02_GET_DATA(pins);
  }
  fixture->pins = pins;
  if (trace_run) {
    trace(fixture);
  }
}

static void reset(struct w65c02_test *fixture) {
  fixture->pins |= W65C02_RES;
  for (int k = 0; k < 7; k++) {
    stepOne(fixture);
  }
}

UTEST_F_SETUP(w65c02_test) {
  (void)utest_result;
  utest_fixture->pins =
      w65c02_init(&utest_fixture->cpu, &(w65c02_desc_t){.bcd_disabled = false});
  memset(utest_fixture->memory, 0, sizeof(utest_fixture->memory));
  trace_run = 0;
}

UTEST_F_TEARDOWN(w65c02_test) {
  (void)utest_result;
  (void)utest_fixture;
}

UTEST_F(w65c02_test, reset) {
  w8(utest_fixture, 0x8000, 0xea);
  w8(utest_fixture, 0x8001, 0xea);
  w8(utest_fixture, 0x8002, 0xea);
  w16(utest_fixture, 0xfffc, 0x8000);
  reset(utest_fixture);
  EXPECT_EQ(W65C02_GET_ADDR(utest_fixture->pins), 0x8000);
}

UTEST_F(w65c02_test, reset_instruction) {
  w8(utest_fixture, 0x8000, 0xce);
  w16(utest_fixture, 0x8001, 0x9000);
  w16(utest_fixture, 0xfffc, 0x8000);
  reset(utest_fixture);
  stepOne(utest_fixture);
  stepOne(utest_fixture);
  stepOne(utest_fixture);
  reset(utest_fixture);
  EXPECT_EQ(W65C02_GET_ADDR(utest_fixture->pins), 0x8000);
}