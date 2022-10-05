
// TX_RS485 LIBRARY
// KRUTIKA NERURKAR

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL
// Target uC:       TM4C123GH6PM
// System Clock:    -

// Hardware configuration:
// UART Interface:
// U1TX (PB1) and U1RX (PB0) are connected to the 2nd controller via RS485 transciever
//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#ifndef TX_RS485_H_
#define TX_RS485_H_

// global variables
extern int32_t msg_in_progress;
extern uint8_t msg_phase; 

// global bits
extern bool cs;
extern bool random;

typedef struct _Tx485msg
{
  uint8_t dst_add;
  uint8_t src_add;
  uint8_t seq_id; 
  uint8_t command;   
  uint8_t channel;               
  uint8_t size_tx;  
  uint8_t data_tx[10]; //this can change
  uint8_t checksum; 
  uint8_t tx_time;
  uint8_t tx_count;
  bool    valid; 

 
}Tx485msg;

# define ack_mask 0x80 // mask
# define nmsg 5        // transmit buffer max value
#define RS485_DEN (*((volatile uint32_t *)(0x42000000 + (0x400043FC-0x40000000)*32 + 4*7)))// DEN enable
#define RS485_TX_LED (*((volatile uint32_t *)(0x42000000 + (0x400043FC-0x40000000)*32 + 4*5)))// TX_LED enable
// global array declaration
extern Tx485msg tx_485msg[];
// global declaration of transit index
extern uint8_t index;
//
extern char str[];
// Transmit functions
void inittx_RS485();

void SendRS485( uint8_t dstadd,uint8_t cmd,uint8_t chan, uint8_t size,uint8_t data[],bool ack);

void SendRS485Byte(void);

void SendAck();

void check_random ();

uint8_t rand_gen(uint8_t node_add);

 
#endif







