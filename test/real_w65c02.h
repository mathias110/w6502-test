#pragma once

#include <stdint.h>
#include <stddef.h>

typedef struct PinState_
{
    /* Inputs */
    uint8_t rst;
    uint8_t phi2;
    uint8_t irq;
    uint8_t nmi;
    /* Input/output */
    uint8_t rdy;

    /* Output */
    uint8_t sync;
    uint8_t vpb;
    uint8_t mlb;
    uint8_t rw;
    uint16_t addr;
    uint8_t data;
} PinState;

typedef struct ProgramFragment_
{
    uint16_t baseAddress;
    uint16_t count;
    uint8_t data[256];
} ProgramFragment;

typedef struct PinTestSuite_ {
    PinState* pinstate;
    size_t cycles;
} PinTestSuite;