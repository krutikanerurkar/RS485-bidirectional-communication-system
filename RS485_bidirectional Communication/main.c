
// Project 2021
// Krutika Nerurkar

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL Evaluation Board
// Target uC:       TM4C123GH6PM
// System Clock:    40 MHz

// Hardware configuration:
// UART Interface:
//   U0TX (PA1) and U0RX (PA0) are connected to the 2nd controller
//   The USB on the 2nd controller enumerates to an ICDI interface and a virtual COM port
//   Configured to 115,200 baud, 8N1

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include "mystring.h"
#include <uart0.h>
#include "uart1.h"
#include "clock.h"
#include "tm4c123gh6pm.h"
#include "commands.h"
#include "tx_RS485.h"
#include "rx_RS485.h"
#include "timer0.h"
#include "adc0.h"

_Bool cs = false;
_Bool random = false;

# define  set_size    1
# define  pulse_size  3
# define  square_size 7
# define  brodcast_add 0xFF

// Initialize Hardware
void initHw()
{
    // Initialize system clock to 40 MHz
    initSystemClockTo40Mhz();

}
int main(void)
{
    // declaring local variables
    USER_DATA data_uirx;
    bool ack = false;
     uint8_t dstadd;
     uint8_t chan;
     uint8_t size;
     uint8_t data[10];
     uint16_t data_2b;
     //Initializing
      initHw();// system clock
     initUart0();// user interface configuration
    initUart1txrx();//uart1 configuration
     init_timer0();// timer configuration
     inittx_RS485();// transmitter initialization "marking messages as invalid";
     initrx_RS485();// receiver
    initAdc0Ss3();
     //putsUart0("System is Ready\n");


    while(1)
    {
      getsUart0(&data_uirx);
      putsUart0(data_uirx.buffer);
      parseFields(&data_uirx);
     _Bool valid_1 = false;

    if(isCommand(&data_uirx, "reset" , 0))
     {
       NVIC_APINT_R = 0x05FA0004;
     }

    if(isCommand(&data_uirx, "POL" , 0))// Poll to everyone
     {
       dstadd =  brodcast_add;
         chan = 0;
         size = 0;
       data[0] = 0;
        valid_1 = true;
        SendRS485(dstadd,Poll_Request,chan,size,data,ack);
     }

      if(isCommand(&data_uirx, "POL" , 1))// poll to address
      {
        dstadd = getFieldInteger(&data_uirx,1);
         chan = 0;
         size = 0;
         data[0] = 0;
        valid_1 = true;
        SendRS485(dstadd,Poll_Request,chan,size,data,ack);
     }


         if (isCommand(&data_uirx, "reset" ,1 ))// reset
         {

          dstadd = getFieldInteger(&data_uirx,1);
          chan = 0;
          size = 0;
          data[0] = 0;
          valid_1 = true;
          SendRS485(dstadd,Reset,chan,size,data,ack);
       }

     if (isCommand(&data_uirx, "CS" , 1))
        {
          char* str = getFieldString(&data_uirx, 1);
           if((strcompare(str,"ON")==0))
              {
                 cs = true;
              }
           if((strcompare(str,"OFF")==0))
             {
                cs = false;
             }
          else
             valid_1 = false;
           }
        if (isCommand(&data_uirx, "random" , 1))
         {
          char* str = getFieldString(&data_uirx, 1);
          if((strcompare(str,"ON")==0))
              {
                 random = true;
              }
         if((strcompare(str,"OFF")==0))

            {
               random = false;
            }
         else
           valid_1 = false;
       }

       if (isCommand(&data_uirx, "ack" , 1))
         {
         char* str = getFieldString(&data_uirx, 1);

           if((strcompare(str,"ON")==0))
              {
                 ack = true;
              }
           if((strcompare(str,"OFF")==0))
             {
                ack = false;
             }
           else
             valid_1 = false;
        }

     if (isCommand(&data_uirx, "sa" ,2 ))

        {
            dstadd = getFieldInteger(&data_uirx,1);
            chan = 0;
            size = 0;
            data[0] = getFieldInteger(&data_uirx,2);
            valid_1 = true;
            SendRS485(dstadd,Set_Address,chan,size,data,ack);

        }




       if (isCommand(&data_uirx, "set" ,3 ))
       {
         dstadd     = getFieldInteger(&data_uirx,1);
         chan       = getFieldInteger(&data_uirx,2);
         data[0]    = getFieldInteger(&data_uirx,3);
         valid_1    = true;
         SendRS485(dstadd,Set,chan,set_size,data,ack);
       }


        if(isCommand(&data_uirx,"Pulse",4))

       {
         dstadd= getFieldInteger(&data_uirx,1);
         chan  = getFieldInteger(&data_uirx,2);
         data[0] = getFieldInteger(&data_uirx,3);
         data_2b = getFieldInteger(&data_uirx,4);
         data[1] = ((data_2b>>8)& 0xFF);
         data[2] = (data_2b & 0xFF);
         valid_1 = true;
         SendRS485(dstadd,Pulse,chan,pulse_size,data,ack);
       }

       if(isCommand(&data_uirx,"Square",7))
       {
           dstadd= getFieldInteger(&data_uirx,1);
           chan  = getFieldInteger(&data_uirx,2);
           data[0] = getFieldInteger(&data_uirx,3);//Value1
           data[1] = getFieldInteger(&data_uirx,4);//Value 2
           data[2] = getFieldInteger(&data_uirx,5);//Time1
           data_2b = getFieldInteger(&data_uirx,6);//Time2
           data[3] = ((data_2b>>8)& 0xFF);
           data[4] = (data_2b & 0xFF);
           data_2b = getFieldInteger(&data_uirx,7);// cycles
           data[5] = ((data_2b>>8)& 0xFF);
           data[6] = (data_2b & 0xFF);
           SendRS485(dstadd,Square,chan,square_size,data,ack);

       }


       if (isCommand(&data_uirx, "alert" , 1))
       {
           char* str = getFieldString(&data_uirx, 1);
           putsUart0("Executed Command: \t");
           putsUart0(str);
           valid_1 = true;
           putcUart0('\n');
       }


       if (valid_1== false)
       {
          putcUart0('\n');
          putsUart0("Invalid command\n");
      }
   }

   // while(true);

    return 0;
}
