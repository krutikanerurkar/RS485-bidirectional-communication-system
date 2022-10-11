
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

#ifndef UART1_H_
#define UART1_H_

#include <stdbool.h>
//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

void initUart1txrx();
int readEEPROM();
void writeEEPROM(uint8_t newaddress);

//void UART1ISR();

#endif

