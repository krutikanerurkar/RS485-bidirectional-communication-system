
// UART1 Library
// Krutika Nerurkar

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL
// Target uC:       TM4C123GH6PM
// System Clock:    -

// Hardware configuration:
// UART Interface:
//   U1TX (PB1) and U1RX (PB0) are connected to the 2nd controller
//   The USB on the 2nd controller enumerates to an ICDI interface and a virtual COM port

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#ifndef TIMER0_H_
#define TIMER0_H_
//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

extern uint32_t tx_ledtimeout;
extern uint32_t rx_ledtimeout;
extern uint8_t  test_cs;
extern bool     test_done;

extern bool     busy;


void init_timer0();
#endif

