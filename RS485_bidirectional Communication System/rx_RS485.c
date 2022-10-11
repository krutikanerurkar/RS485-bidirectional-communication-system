// Receive RS485
// Krutika Nerurkar

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL
// Target uC:       TM4C123GH6PM
// System Clock:    -
//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "commands.h"
#include "clock.h"
#include "uart0.h"
#include "uart1.h"
#include "tx_RS485.h"
#include "rx_RS485.h"
#include "timer0.h"
#include <stdio.h>

#define Poll_size 1
 char str[100];

// Bitbanding  of internal LEDs assinging them to particular channel
#define RED_LED      (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 1*4)))
#define GREEN_LED    (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 3*4)))
#define PUSH_BUTTON  (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 4*4)))


 // PortF masks
#define GREEN_LED_MASK 8
#define RED_LED_MASK 2
#define PUSH_BUTTON_MASK 16

void initrx_RS485()// Initialize the internal Leds and switches for controller
   {

     SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;
     _delay_cycles(3);

    // Configure LED and pushbutton pins
     GPIO_PORTF_DIR_R |= GREEN_LED_MASK | RED_LED_MASK;  // bits 1 and 3 are outputs, other pins are inputs
     GPIO_PORTF_DIR_R &= ~PUSH_BUTTON_MASK;
     GPIO_PORTF_DR2R_R |= GREEN_LED_MASK | RED_LED_MASK;  // set drive strength to 2mA (not needed since default configuration -- for clarity)
     GPIO_PORTF_DEN_R |= PUSH_BUTTON_MASK | GREEN_LED_MASK | RED_LED_MASK;
     GPIO_PORTF_PUR_R |= PUSH_BUTTON_MASK;


   }


void process_commands() // Processing the commands in the receiver side

{


 if((rx_485msg.command &0x7F) ==  Set ) // process commands and

 {

    switch(rx_485msg.channel)
    {
    
    case 0 :  RED_LED = rx_485msg.data_rx[rx_485msg.size_rx -1];
              break;
    
    case 1 :  GREEN_LED = rx_485msg.data_rx[rx_485msg.size_rx -1];
              break;

  }
 }

  if((rx_485msg.command &0x7F)  == Data_Request)

   {
    switch(rx_485msg.channel)
    {
    
    case 0:      SendRS485(rx_485msg.src_add,Data_Report,rx_485msg.channel,1,RED_LED,false);
                 break;

    case 1:      SendRS485(rx_485msg.src_add,Data_Report,rx_485msg.channel,1,GREEN_LED,false);
                 break;

    case 2 :    SendRS485(rx_485msg.src_add,Data_Report,rx_485msg.channel,1,PUSH_BUTTON,false);
                 break;
  }



if((rx_485msg.command & 0x7F) == Data_Report)
 {
    sprintf(str, "The channel  :       %2u\n", rx_485msg.data_rx[0]);
    putsUart0(str);
  }

}

 if((rx_485msg.command & 0x7F) == Set_Address)
 {
    writeEEPROM( rx_485msg.data_rx[0]);
 }


if((rx_485msg.command & 0x7F) == Reset)
{

    NVIC_APINT_R = 0x05FA0004;

}

if((rx_485msg.command & 0x7F) == Poll_Request)
  {
    SendRS485(rx_485msg.src_add,Poll_Response,0,Poll_size,rx_485msg.dst_add,false);
  }

if((rx_485msg.command & 0x7F) == Poll_Response)
{


    sprintf(str, "The address is  :       %2u\n", rx_485msg.data_rx[0]);
    putsUart0(str);
}

}


void process(Rx485msg* rx_485msg)// process the data at the receiver end
{
  if(((rx_485msg -> command) & ack_mask ) == ack_mask)
   {

     SendAck();
     process_commands();
   }
  else
    process_commands();

 if(rx_485msg-> command == Acknowledge)

{

    uint8_t i = 0;
// check this
    for(i=0;i<nmsg;i++)
   {
    if(rx_485msg -> data_rx[0] == tx_485msg[i].seq_id && rx_485msg->dst_add == tx_485msg[i].src_add )

    {

     tx_485msg[i].valid = false;
  
     putsUart0("Acknowledgment for seq_id :\t");
     uint8_t c = rx_485msg->data_rx[0];
     putcUart0(c);
     putsUart0("Received\n");
    }
   }
  }
}




