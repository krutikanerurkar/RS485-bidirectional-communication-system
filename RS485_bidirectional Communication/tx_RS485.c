// Transmit RS485
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
#include <timer0.h>
#include "tm4c123gh6pm.h"
#include "commands.h"
#include "clock.h"
#include "tx_RS485.h"
#include "rx_RS485.h"
#include "uart1.h"
#include "adc0.h"
#include "math.h"

// variables
# define Max_count 5
uint8_t min_back_off_time = 10;
uint8_t T = 10;

//#define RS485_DEN 128 (PA7) is used
#define RS485_DEN (*((volatile uint32_t *)(0x42000000 + (0x400043FC-0x40000000)*32 + 4*7)))
#define RS485_TX_LED (*((volatile uint32_t *)(0x42000000 + (0x400043FC-0x40000000)*32 + 4*5)))// red led PA5
#define RS485_RX_LED (*((volatile uint32_t *)(0x42000000 + (0x400243FC-0x40000000)*32 + 4*5)))// green led PE5
//#define TX_LED


int32_t msg_in_progress = -1;
uint8_t msg_phase;
uint8_t index = 0; // transmit index;



//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

// Initialize tx_RS485 message buffer with zero
Tx485msg tx_485msg[nmsg];
void inittx_RS485()
{
      uint8_t i;
      for(i=0;i<nmsg;i++)
      {
        tx_485msg[i].valid = false;
      }
}




// sends the byte to the RS485 data Register(UART1) for transmission
void SendRS485Byte()
{


  if (msg_in_progress == -1)

     {
      if(tx_485msg[index].valid== true && tx_485msg[index].tx_time==0 && index <nmsg )
     {

           msg_phase = 0;
           msg_in_progress = index;
           index++;
     }
    }


   if (msg_in_progress > -1)
   {

         RS485_DEN =1;
        if (msg_phase == 0)
        {

              if(cs == true)// check if cs is true
              {
               if(test_done == true)
               {
                test_done = false;
                if(!busy)
                {
                 // UART1_LCRH_R|= ~(UART_LCRH_EPS); // parity bit 1
                  UART1_DR_R = tx_485msg[msg_in_progress].dst_add;
                  msg_phase++;
                }

               }
             }
              else
              {
               test_done = false;
               busy = false;
               test_cs = false;

              UART1_DR_R = tx_485msg[msg_in_progress].dst_add;
             msg_phase++;
            /* }*/
             }
        }
       else if (msg_phase == 1)

        {
          //UART1_LCRH_R|= (UART_LCRH_EPS);// parity bit 0
          UART1_DR_R = tx_485msg[msg_in_progress].src_add;
          msg_phase++;
        }

      else if (msg_phase == 2)
        {
         UART1_DR_R = tx_485msg[msg_in_progress].seq_id;
         msg_phase++;
        }
     else if (msg_phase == 3)
        {
         UART1_DR_R = tx_485msg[msg_in_progress].command;
         msg_phase++;
        }
     else if (msg_phase == 4)
      {
        UART1_DR_R = tx_485msg[msg_in_progress].channel;
        msg_phase++;
      }
    else if (msg_phase == 5)
      {
        UART1_DR_R = tx_485msg[msg_in_progress].size_tx;
       msg_phase++;
      }

   else if(msg_phase == 6)
   {
       static  uint8_t p = 0;
      if(p<tx_485msg[msg_in_progress].size_tx)
         {
          UART1_DR_R =tx_485msg[msg_in_progress].data_tx[p];
          p++;
         }

      if(p == tx_485msg[msg_in_progress].size_tx)
      {
        msg_phase++;
      }
    }
  else if(msg_phase == 7)
   {
      UART1_DR_R = tx_485msg[msg_in_progress].checksum;
       //RS485_DEN = 0;

    if((( tx_485msg[msg_in_progress].command & ack_mask) == 0x00))// no acknowlegement
       {
           RS485_TX_LED = 1;
          tx_ledtimeout = 3;
          msg_in_progress=-1;
         tx_485msg[index].valid = false;

         // enable the timer.
       }
   else
     {
          (tx_485msg[msg_in_progress].tx_count)++;
          if(tx_485msg[msg_in_progress].tx_count == Max_count)
          {
            RS485_TX_LED = 1;
            tx_485msg[msg_in_progress].valid = false;
             msg_in_progress = -1;
            //failed to send the command (print msg)
          }
         else
        check_random();// check if random is on or off for proper retransmission
     }
   }
 }

  //else
      //if()// not transmit or receiving
}     //DEN=0

// Stores the data in the user created transmit buffer
void SendRS485(uint8_t dstadd,uint8_t cmd,uint8_t chan, uint8_t size,uint8_t data[], bool ack)
{
       static uint8_t  i = 0;
       uint8_t j = 0;
   while(tx_485msg[i].valid == false && i <nmsg)
    {
      uint8_t sum = 0;
      tx_485msg[i].dst_add = dstadd;
      sum += dstadd;
      tx_485msg[i].src_add = 1;
      sum += 1;
     if(ack == true)
      {
        tx_485msg[i].command = (cmd|ack_mask);
        sum += tx_485msg[i].command;
      }
    else
     {
       tx_485msg[i].command = cmd;
       sum += cmd;
     }

   tx_485msg[i].channel = chan;
    sum += chan;
   tx_485msg[i].size_tx = size;
   sum += size;
    for( j=0; j<size;j++)
      {
          tx_485msg[i].data_tx[j] = data[j];
          sum += data[j];
      }

      tx_485msg[i].seq_id   = i;   //(or use modulo 256)
      sum += i;

      tx_485msg[i].checksum = ~(sum);

      tx_485msg[i].tx_time  = 0;

      tx_485msg[i].tx_count = 0;

     tx_485msg[i].valid    = true;

     i++;
      while(UART1_FR_R & UART_FR_TXFF);// while (UART1_FR_R & UART_FR_TXFF); // If UART is wait if uart0 tx fifo full // while (~(UART1_FR_R & UART_FR_TXFE));                             // TX_FF = 1 (transmitter still has data to transfer)                 TXFE = 1 (transmitter has no data to transfer)
     SendRS485Byte();
     break;
   }

}

uint8_t rand_gen(uint8_t node_add) // random generator using inbuilt temp sensor (inbuilt thermal noise generation)
{


          uint8_t N = tx_485msg[msg_in_progress].tx_count;
          setAdc0Ss3Mux(3);
          uint16_t seed = readAdc0Ss3();
          uint8_t buffer = 0;
          uint8_t var = 0;
          uint8_t bit[8];
          uint8_t bit_index;
          buffer = (seed ^(0x5555));// equal distribution
          buffer = (buffer + node_add);

          while(1)
          {

          for(bit_index=0;bit_index<8;bit_index++)
           {
             uint8_t mask = 0;

             mask|= 1<< bit_index;

            if((buffer & mask)== 0)
             {
               bit[bit_index]= 0;
               var ^= bit[bit_index];
             }
            else
            {
                bit[bit_index]= 1;
                var ^= bit[bit_index];
            }

           }

           buffer = ( buffer >> 1)| (var<<7);
            if(0 < buffer < pow(2,N)*T)
             return buffer;
             else
             {
              var = 0;
              continue;
            }
         }
}


void check_random()
    {

    if(random == false)
     {
      uint8_t N = tx_485msg[index].tx_count;

      tx_485msg[index].tx_time = min_back_off_time + pow(2,N)*T;
      msg_in_progress = -1;
      //timer enable


     }
    else
    {

     tx_485msg[index].tx_time = min_back_off_time + rand_gen(tx_485msg[index].dst_add);// random on
     msg_in_progress = -1;
        //Timer enable

    }

 }


// Sends back Ack to the sender if (Ack is required by the sender "Ack ON")
void SendAck()
   {
    SendRS485( rx_485msg.src_add,Acknowledge,0,1,rx_485msg.seq_id,false);

   }



      

