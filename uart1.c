// UART1 ( Transmit and Receive ISR)
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
#include "timer0.h"



//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
Rx485msg rx_485msg;
uint8_t rx_phase = 0;
bool busy = false; // for carrier sense
uint8_t myAddress;

// PortB UART1 masks
#define UART1_TX_MASK 2
#define UART1_RX_MASK 1

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------



void initUart1txrx()
{

    // Set GPIO ports to use APB (not needed since default configuration -- for clarity)
    SYSCTL_GPIOHBCTL_R = 0;

    // Enable clocks
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R1;
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;
   SYSCTL_RCGCEEPROM_R =   SYSCTL_RCGCEEPROM_R0;
   // SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;
    _delay_cycles(3);

    // Configure DEN pin
  //  GPIO_PORTA_DIR_R |= RS485_DEN;
  //  GPIO_PORTA_DR2R_R|=RS485_DEN ;
  //  GPIO_PORTA_DEN_R | RS485_DEN;





    // Configure UART1 pins

    GPIO_PORTB_DIR_R |= UART1_TX_MASK;                   // enable output on UART1 TX pin
    GPIO_PORTB_DIR_R &= ~UART1_RX_MASK;                   // enable input on UART1 RX pin
    GPIO_PORTB_DR2R_R |= UART1_TX_MASK;                // set drive strength to 2mA (not needed since default configuration -- for clarity)
    GPIO_PORTB_DEN_R |= UART1_TX_MASK | UART1_RX_MASK;   // enable digital on UART1 pins
    GPIO_PORTB_AFSEL_R |= UART1_TX_MASK | UART1_RX_MASK;  // use peripheral to drive PB0, PB1
    GPIO_PORTB_PCTL_R &= ~(GPIO_PCTL_PB1_M | GPIO_PCTL_PB0_M); // clear bits 0-7
    GPIO_PORTB_PCTL_R |= GPIO_PCTL_PB1_U1TX | GPIO_PCTL_PB0_U1RX;
                                                        // select UART0 to drive pins PA0 and PA1: default, added for clarity

    // Configure UART1 to 38400 baud, 8N1 format, 1 parity bit (with SPS = 1,EPS = 0), 1 stop bit
    UART1_CTL_R = 0;                                    // turn-off UART1 to allow safe programming
    UART1_CC_R = UART_CC_CS_SYSCLK;                     // use system clock (40 MHz)
    UART1_IBRD_R = 65;                                  // r = 40 MHz / (Nx115.2kHz), set floor(r)=21, where N=16
    UART1_FBRD_R = 7;                                  // round(fract(r)*64)=45
    UART1_LCRH_R |= UART_LCRH_SPS|UART_LCRH_WLEN_8 | UART_LCRH_PEN; // configure for 8N1 w/ 16-level FIFO
    UART1_ICR_R |= UART_ICR_TXIC|UART_ICR_RXIC ;// clear the interrupts
    UART1_IM_R = UART_IM_TXIM|UART_IM_RXIM;// Interrupt masks
    UART1_CTL_R = UART_CTL_TXE | UART_CTL_RXE|UART_CTL_LBE| UART_CTL_EOT |UART_CTL_UARTEN;// enable TX, RX, and module// LBE enabled
    NVIC_EN0_R|= 1<< (INT_UART1-16);// enable the

    // EEPROM configuration
    EEPROM_EEBLOCK_R = 0;
    EEPROM_EEOFFSET_R = 0;



}

void UART1ISR()
{
  if((UART1_RIS_R & UART_RIS_TXRIS)==UART_RIS_TXRIS)// Send the data to the UART data register when the ISR occurs (transmitter ISR)
  {
    SendRS485Byte();

   UART1_ICR_R |= UART_ICR_TXIC;
  }

  if((UART1_RIS_R & UART_RIS_RXRIS)==UART_RIS_RXRIS)// don't need to store  the dst add as it is already stored in eeprom (checking status of RXRIS)) (receiver ISR)
   {
     //busy = true;
     static uint8_t sum = 0;
            uint8_t fcheck = 0;

     // myAddress = readEEPROM();
          //  uint8_t  dst_add = (UART1_DR_R & 0xFF);

    if ((rx_phase == 0) && (!((( UART1_DR_R  &  UART_DR_PE) >> 9) ^
           ((UART1_LCRH_R & UART_LCRH_EPS) >> 2))) && ( ( UART1_DR_R & 0xFF) == 0xFF||( UART1_DR_R & 0xFF)== 1))// default add = 1;
   {

    rx_485msg.dst_add = (UART1_DR_R & 0xFF);
    sum += rx_485msg.dst_add;
    rx_phase++;

   }

   else if(rx_phase == 1)
      {

       rx_485msg.src_add = (UART1_DR_R & 0xFF);
       sum += rx_485msg.src_add;
       rx_phase++;
      }

  else if (rx_phase == 2)

       {
          rx_485msg.seq_id = (UART1_DR_R & 0xFF);
          sum += rx_485msg.seq_id;
          rx_phase++;
       }

  else if (rx_phase == 3)

      {
          rx_485msg.command = ( UART1_DR_R & 0xFF);
          sum += rx_485msg.command;
          rx_phase++;
      }
  else if (rx_phase == 4)

      {
         rx_485msg.channel = ( UART1_DR_R & 0xFF);
         sum += rx_485msg.channel;
         rx_phase++;
      }
  else if (rx_phase == 5)

      {
        rx_485msg.size_rx = ( UART1_DR_R & 0xFF);
        sum += rx_485msg.size_rx;
        rx_phase++;
      }
  else if(rx_phase == 6)
     {

      static  uint8_t m = 0;
      //if(m<rx_485msg.size_rx )
      if(m< rx_485msg.size_rx)
      {
        rx_485msg.data_rx[m] = ( UART1_DR_R & 0xFF);
        sum += rx_485msg.data_rx[m];
        m++;
      }
      if(m == rx_485msg.size_rx)
      {
          rx_phase ++;
      }
    }
    else if(rx_phase == 7)
    {
       rx_485msg.checksum = ( UART1_DR_R & 0xFF);
       sum += rx_485msg.checksum;
       fcheck = ~(sum);
       rx_phase = 0;
      if(fcheck == 0)
      {
        RS485_RX_LED = 1;// process(&rx_485msg);
        rx_ledtimeout = 3;
        process(&rx_485msg);

      }
      else
          RS485_RX_LED = 1; // turn on led // receiver is not good

    }
   }
  UART1_ICR_R |= UART_ICR_RXIC;
}

int readEEPROM()
{
    if(EEPROM_EERDWR_R == 0xFFFFFFFF)
    {
        myAddress = 1;
    }
    else
    {
        myAddress = EEPROM_EERDWR_R;                           // source address from EEPROM
    }
    return myAddress;
}

void writeEEPROM(uint8_t newaddress)
{
    while(EEPROM_EEDONE_R & EEPROM_EEDONE_WORKING);
    EEPROM_EEOFFSET_R = 0;
    EEPROM_EERDWR_R = rx_485msg.data_rx[0];
}
