RS-485 based bidirectional communication system(peer to peer communication)(TM4C1234GH6PM)

In this repository I have attached my code for the same. I have used a modular design approach.

Following are the steps that will help you through the execution of the code . Each functionality has a source code and header file associated with them.

The user takes data from the putty terminal and process it using the parse string function created by the user(Uart0.c/Uart0.h/main.c)

The parse data is then further extracted and send to the is Command function created by the user where the commands are processed further(Uart0.c/Uart.h)

After the commands have been processed the data is extracted from them and further sent to the transmit buffer for transmission. RS485 interface is used for this purpose(tx_485.c/tx_485.h)

For, this UART 1 (FIFO disabled) with a interrupt driven approach is used.The UART 1 ISR is used for checking the raw interrupt status so that we can transmit and receive in accordingly.(Uart1.c/Uart1.h)

The data at the receiver end is stored in the receiver buffer and then further (process message) is used to process the received commands and implement the functionality.(rx_485.c/rx_485.h,Uart1.c/Uart1.h)

When the commands are received they are checked to see which functionality should be applied and necessary actions is taken on the connected input(rx_485.c/rx_485.h)

If the command is received with an “Ack on” from the sender, Acknowledgement is send back to the user.(rx_485.c/tx_485.c)

Also, in order to keep a check if the receiver sends the acknowledgement  within a given time frame.
There is a Random ON and Random OFF command which helps to calculate the retransmission time.For, Random ON command we have used PN generation method to calculate the retransmission time.
This is advisable so that there are no collisions during transmissions from different receiver at the same time as it is a peer to peer network.(tx_485.c/tx_485.h)

For, generation of random numbers we have used a internal temperature sensor to generate random seed which is then further processed to create a random number.(tx_485.c/tx_485.h,adc0.c/adc.h)

There is also a Timer 0 ISR running at 100 Hz which is basically used to blink the Red and green led for receiver and transmitter and for Carrier Sense and Random number generation(timer0.c/timer0.h)

All the commands which are used in this project have been mentioned in the commands.h library
