// UART0  ( User Interface Functions)
// Krutika Nerurkar

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL
// Target uC:       TM4C123GH6PM
// System Clock:    -

// Hardware configuration:
// UART Interface:
//   U0TX (PA1) and U0RX (PA0) are connected to the 2nd controller
//   The USB on the 2nd controller enumerates to an ICDI interface and a virtual COM port

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "uart0.h"
#include "clock.h"
#include<string.h>
#include"mystring.h"
// PortA masks
#define UART_TX_MASK 2
#define UART_RX_MASK 1
#define RS485_DEN_M 128
#define RS485_TX_LED_M 32
#define RS485_RX_LED_M 32
#define AIN3_MASK 1
//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

// Initialize UART0
void initUart0()
{
    // Configure HW to work with 16 MHz XTAL, PLL enabled, system clock of 40 MHz
   // SYSCTL_RCC_R = SYSCTL_RCC_XTAL_16MHZ | SYSCTL_RCC_OSCSRC_MAIN | SYSCTL_RCC_USESYSDIV | (4 << SYSCTL_RCC_SYSDIV_S);

    // Set GPIO ports to use APB (not needed since default configuration -- for clarity)
    SYSCTL_GPIOHBCTL_R = 0;

    // Enable clocks
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R0;
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;

    _delay_cycles(3);


    // Configure AIN3 as an analog input
         GPIO_PORTE_AFSEL_R |= AIN3_MASK;                 // select alternative functions for AN3 (PE0)
         GPIO_PORTE_DEN_R &= ~AIN3_MASK;                  // turn off digital operation on pin PE0
         GPIO_PORTE_AMSEL_R |= AIN3_MASK;                 // turn on analog operation on pin PE0

    // Configure UART0 pins and TX_LED and RX_LED
    GPIO_PORTA_DIR_R |= UART_TX_MASK| RS485_DEN_M| RS485_TX_LED_M;               // enable output on UART0 TX pin
    GPIO_PORTA_DIR_R &= ~UART_RX_MASK;                   // enable input on UART0 RX pin
    GPIO_PORTA_DR2R_R |= UART_TX_MASK|RS485_DEN_M|RS485_TX_LED_M;                  // set drive strength to 2mA (not needed since default configuration -- for clarity)
    GPIO_PORTA_DEN_R |= UART_TX_MASK | UART_RX_MASK| RS485_DEN_M |RS485_TX_LED_M;   // enable digital on UART0 pins
    GPIO_PORTA_AFSEL_R |= UART_TX_MASK | UART_RX_MASK;  // use peripheral to drive PA0, PA1
    GPIO_PORTA_PCTL_R &= ~(GPIO_PCTL_PA1_M | GPIO_PCTL_PA0_M); // clear bits 0-7
    GPIO_PORTA_PCTL_R |= GPIO_PCTL_PA1_U0TX | GPIO_PCTL_PA0_U0RX;
    GPIO_PORTE_DIR_R |= RS485_RX_LED_M;
    GPIO_PORTE_DR2R_R|= RS485_RX_LED_M;// select UART0 to drive pins PA0 and PA1: default, added for clarity
    GPIO_PORTE_DEN_R |=  RS485_RX_LED_M;

    // Configure UART0 to 115200 baud, 8N1 format
    UART0_CTL_R = 0;                                    // turn-off UART0 to allow safe programming
    UART0_CC_R = UART_CC_CS_SYSCLK;                     // use system clock (40 MHz)
    UART0_IBRD_R = 21;                                  // r = 40 MHz / (Nx115.2kHz), set floor(r)=21, where N=16
    UART0_FBRD_R = 45;                                  // round(fract(r)*64)=45
    UART0_LCRH_R = UART_LCRH_WLEN_8 | UART_LCRH_FEN;    // configure for 8N1 w/ 16-level FIFO
    UART0_CTL_R = UART_CTL_TXE | UART_CTL_RXE | UART_CTL_UARTEN;   // enable TX, RX, and module
}
//

// Blocking function that writes a serial character when the UART buffer is not full
void putcUart0(char c)
{
    while (UART0_FR_R & UART_FR_TXFF);               // wait if uart0 tx fifo full
    UART0_DR_R = c;                                  // write character to fifo
}

// Blocking function that writes a string when the UART buffer is not full
void putsUart0(char* str)
{
    uint8_t i = 0;
    while (str[i] != '\0')
    putcUart0(str[i++]);
}

// Blocking function that returns with serial data once the buffer is not empty
char getcUart0()
{    while (UART0_FR_R & UART_FR_RXFE);               // wait if uart0 rx fifo empty
     return UART0_DR_R & 0xFF;                        // get character from fifo
}

// Returns the status of the receive buffer
bool kbhitUart0()
{
    return !(UART0_FR_R & UART_FR_RXFE);
}

//Blocking function that returns a string once the buffer is not empty

void getsUart0(USER_DATA* data_uirx)
{
  int count = 0;

 while(1)
 {

  char c = getcUart0();// get char


  if (c == 8 || c==127) // execute if backspace
  {
      if (count > 0)
    {
      count --;
      continue;
    }
     else
     continue;
  }
 else if(c==13||c==10) //execute if in line or carriage return
      {
        data_uirx->buffer[count]='\0';
        break;
      }
 else if(c>=32)  // execute if other characters
      {
        data_uirx->buffer[count++]= c ;

      if (count == MAX_CHARS)
      {
       data_uirx->buffer[count]='\0';
       break;
      }
      else
      continue;
      }
   }
}

// Parsing of string

void parseFields(USER_DATA*  data_uirx)
{
 ( data_uirx->fieldCount) = 0;
 uint8_t i;
 char str[82];
 str[0]=32;// assume that the string starts with delimeter (here delimeter is space whose Ascii value is 32)
 for (i=0; data_uirx->buffer[i]!='\0';i++)// save data from the data  buffer (in structure ) into another array (where the first character is a delimeter)

     {
           str[i+1]=  data_uirx->buffer[i];
     }
 for(i=0;(str[i]!='\0'&& ( data_uirx->fieldCount)!=MAX_FIELDS) ;i++)// use the data saved in the buffer for further processing
 {
   if(((str[i+1]>= 65 && str[i+1]<=90) || (str[i+1]>= 97 && str[i+1]<= 122))  && !((str[i]>=65) && (str[i]<=90) ||(str[i]>=97&& str[i]<= 122)))
   // if ((current data received is alpha numeric (A-Z or a-z)) AND (previous received data is not alpha numeric))
        {

       //change the field type and record the field position

       ( data_uirx->fieldType[ data_uirx->fieldCount]) = 'a' ;
       ( data_uirx->fieldPosition[ data_uirx->fieldCount]) = i;
       ( data_uirx->fieldCount)++;
       str[i] = str[i+1];

       }

   else if((str[i+1]>=48 && str[i+1]<=57) && !(str[i]>=48 && str[i]<=57)) // (if the current data is numeric(0-9) AND ( previous data received is numeric)
    {
     ( data_uirx->fieldType[ data_uirx->fieldCount]) = 'n';
     ( data_uirx->fieldPosition[ data_uirx->fieldCount]) = i;
     ( data_uirx->fieldCount)++;
      str[i] = str[i+1];
    }

  else
       str[i] = str[i+1];

 }
 // load back all data into data buffer
 for (i=0; str[i]!='\0';i++)

   {
     data_uirx->buffer[i] = str[i];
   }
 //  To turn all delimiters to null characters at last when exited from the first loop
   uint8_t j ;
   for( j=0; data_uirx->buffer[j]!='\0';j++)
   {
       while (!(( data_uirx->buffer[j]>= 65 &&  data_uirx->buffer[j]<=90) || ( data_uirx->buffer[j]>= 97 &&  data_uirx->buffer[j]<=122) || ( data_uirx->buffer[j]>=48 &&  data_uirx->buffer[j]<=57)
              || data_uirx->buffer[j]=='\0'))// only executes when a delimeter is present
       {
              data_uirx->buffer[j] = '\0';
        }

      }
   }


// (data->buffer[data->fieldPosition[fieldNumber]])== fieldNumber
uint32_t getFieldInteger(USER_DATA*  data_uirx, uint8_t fieldNumber)// getting the value of the integer sent in the command
   {

    if(( data_uirx->fieldCount>=fieldNumber)  && ( data_uirx->fieldType[fieldNumber]=='n'))
     {
        char* str = &data_uirx->buffer[ data_uirx->fieldPosition[fieldNumber]];
        uint8_t i;
        uint32_t sum = 0;
         for(i=0;str[i]!='\0';i++)
         {

            sum = sum*10 + (str[i]-'0');
         }
        return sum;

     }

    else  return 0;

}
char* getFieldString(USER_DATA*  data_uirx, uint8_t fieldNumber) // getting the executed value of the string in the command
    {
          if( data_uirx->fieldCount>=fieldNumber)
          {
              return (&data_uirx->buffer[ data_uirx->fieldPosition[fieldNumber]]);
          }
          else return '\0';
    }


bool isCommand(USER_DATA*  data_uirx, const char strCommand[], uint8_t minArguments)// to check whether the sent command is correct or not
{

     char* str =  data_uirx->buffer;

    if ((strcompare(str,strCommand)==0) && minArguments < data_uirx->fieldCount)

    {
        return true ;
    }

    else return false;

}




