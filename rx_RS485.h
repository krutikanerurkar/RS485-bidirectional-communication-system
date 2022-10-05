// RX_RS485 LIBRARY
// KRUTIKA NERURKAR

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL
// Target uC:       TM4C123GH6PM
// System Clock:    -

// Hardware configuration:
// UART Interface:
// U1TX (PB1) and U1RX (PB0) are connected to the 2nd controller via RS485 transceiver
//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------
#include <stdint.h>

#ifndef RX_RS485_H_
#define RX_RS485_H_


#define RS485_RX_LED (*((volatile uint32_t *)(0x42000000 + (0x400243FC-0x40000000)*32 + 4*5)))// green led PE5
typedef struct _Rx485msg
{
  uint8_t dst_add;
  uint8_t src_add;
  uint8_t seq_id; 
  uint8_t command; 
  uint8_t channel;   
  uint8_t size_rx;  
  uint8_t data_rx[10]; 
  uint8_t checksum;  
}Rx485msg;

/*typedef struct _action
{

    uint8_t dst_add;
    uint8_t src_add;
    uint8_t seq_id;
    uint8_t command;
    uint8_t channel;
    uint8_t size_rx;
    uint8_t data_rx[10];
   uint8_t checksum;
}*/

extern  Rx485msg rx_485msg;
void initrx_RS485();
void process(Rx485msg* rx_485msg);
void process_commands();
#endif
