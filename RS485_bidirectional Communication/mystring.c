
// STRING LIBRARY
// KRUTIKA NERURKAR

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
#include"mystring.h"
//#include"uart0.h"

// user defined string function

uint32_t strcompare( const char* str1 , const char* str)
{
     
 uint8_t i;
for(i=0;((str1[i]!='\0')|| (str[i]!='\0'));i++)
{


if(str1[i]==str[i])
{
   continue;
}

else
 return 1;

}
 return 0;
}
