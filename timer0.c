
// TIMER0 ( Transmit and Receive ISR)
//Krutika Nerurkar


//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL
// Target uC:       TM4C123GH6PM
// System Clock:    -

// Hardware configuration:
// UART Interface:
//   U1TX (PB1) and U1RX (PB0) are connected to the 2nd controller (via RS485 transceiver)
//   The USB on the 2nd controller enumerates to an ICDI interface and a virtual COM port

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "uart1.h"
#include "clock.h"
#include<string.h>
#include"mystring.h"
#include"tx_RS485.h"
#include"rx_RS485.h"
#include"timer0.h"
 uint32_t tx_ledtimeout  = 0;
 uint32_t rx_ledtimeout  = 0;
 uint8_t  test_cs  = 2;
 bool test_done =  false;



void init_timer0()
{
// Enable clocks
    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R0;
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;
    _delay_cycles(3);
// Configure Timer 1 as the time base
    TIMER0_CTL_R &= ~TIMER_CTL_TAEN;                 // turn-off timer before reconfiguring
    TIMER0_CFG_R = TIMER_CFG_32_BIT_TIMER;           // configure as 32-bit timer (A+B)
    TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;          // configure for periodic mode (count down)
    TIMER0_TAILR_R = 40,0000;                       // set load value to 400000 for 100 Hz interrupt rate
    TIMER0_CTL_R |= TIMER_CTL_TAEN;                  // turn-on interrupts for timeout in timer module                  // turn-on timer
    NVIC_EN0_R |= 1 << (INT_TIMER0A-16);             // turn-on interrupt 37 (TIMER1A) in NVIC
}








void Timer0ISR()

{
 uint8_t i;
if (test_cs>0)
{
     (test_cs)--;
   if(test_cs==0)
      test_done = true;
}


for(i=0;i<nmsg;i++)
{
 if(tx_485msg[i].valid == true)
 {
  if(tx_485msg[i].tx_time > 0)
 {
   (tx_485msg[i].tx_time)--;

 if(tx_485msg[i].tx_time==0)
   {
     if(UART1_FR_R & UART_FR_TXFF);
     if((UART1_RIS_R & UART_RIS_TXRIS)==0x00)
          SendRS485Byte();

    }
  }
 }
}

if(tx_ledtimeout>0)
  {
   (tx_ledtimeout)--;
   if(tx_ledtimeout==0)
      RS485_TX_LED = 0;
  }

if(rx_ledtimeout>0)
 
  {
   (rx_ledtimeout)--;
   if(rx_ledtimeout==0)
       RS485_RX_LED = 0;
  }
// process messages


}
