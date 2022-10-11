// COMMANDS LIBRARY
// KRUTIKA NERURKAR

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL
// Target uC:       TM4C123GH6PM
// System Clock:  

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#ifndef COMMANDS_H_
#define COMMANDS_H_

#include <stdint.h>


enum ControlCommand
{
   Set          = 0x00,
   Piecewise    = 0x01,
   Pulse        = 0x02,
   Square       = 0x03,
   Sawtooth     = 0x04,
   Triangle     = 0x05,   
  // Set_ack      = 0x80,
 // Piecewise_ack = 0x81,
 //  Pulse_ack    = 0x82,
 //  Square_ack   = 0x83,
 //  Sawtooth_ack = 0x84,
  // Triangle_ack = 0x85,

};

enum DataCommand
{
    Data_Request       = 0x30,
    Data_Report        = 0x31,
    Report_Control     = 0x32,
   // Data_Request_ack   = 0xB0,
   // Data_Report_ack    = 0xB1,
   // Report_Control_ack = 0xB2,
};

enum UICommand
{

   LCD_Display_Text     = 0x40,
   RGB                  = 0x48,
   RGB_Piecewise        = 0x49,
  // LCD_Display_Text_ack = 0xC0,
  // RGB_ack              = 0xC8,
  // RGB_Piecewise_ack    = 0xC9,
};

enum SerialCommand
{

   UART_Data          = 0x50,
   UART_Control       = 0x51,
   I2C_Command        = 0x52,
//   UART_Data_ack      = 0xD0,
//  UART_Control_ack   = 0xD1,
//   I2C_Command_ack    = 0xD2,

};


enum SystemCommand
{

   Acknowledge       = 0x70,
   Poll_Request      = 0x78,
   Poll_Response     = 0x79,
   Set_Address       = 0x7A,
   Node_Control      = 0x7D,
   Bootload          = 0x7E,
   Reset             = 0x7F,

};

#endif





