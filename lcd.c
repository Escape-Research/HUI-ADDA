/*
 * File:   lcd.c
 * Author: sm186105
 *
 * Created on July 11, 2017, 2:06 PM
 */


#include "xc.h"

#include "user.h"
#include "lcd.h"

char gLCDBuffer[2][8] = { };

/*   Port Mapping
     
   LCD D4:   PortB:10
   LCD D5:   PortB:11
   LCD D6:   PortB:12
   LCD D7:   PortB:13

   LCD E :   PortB:14
   LCD DI:   PortB:15
   LCD RW:   GND 
*/


void initializeLCD()
{
    // Zero out all lines to LCD
    LATBbits.LATB10 = 0;
    LATBbits.LATB11 = 0;
    LATBbits.LATB12 = 0;
    LATBbits.LATB13 = 0;
    LATBbits.LATB14 = 0;
    LATBbits.LATB15 = 0;
    
    // Wait a minimum of 40 ms after power-up before we do anything 
    __delay_ms(40);
    
    
}

void sendLCDCommand(char cCommand)
{
    
}

void queueLCDCharacter(char cChar)
{
    
}

void writeLCDString(unsigned int row, unsigned int column, char *pString)
{
    // Initialize row and column counters
    int r = row;
    int c = column;
    
    // Get the first character
    char ch = *pString;
    
    // Iterate through the string until we reach value 0
    while (ch != 0)
    {
        // Check if we need to move to the next row
        if (c > 7)
        {
            r++;    // Next row
            c = 0;  // Start from column 0 again
        }
        
        // Out of bounds
        if (r > 1)
            return;
        
        // Store the character in the buffer
        gLCDBuffer[r][c] = ch;
        
        // Increment the column count
        c++;
        
        // Move to the next character
        pString++;
        ch = *pString;
    }
}

void clearLCDScreen()
{
    // Fill the buffer with spaces
    
    int i, j;
    for (i = 0; i < 2; i++)
        for (j = 0; j < 8; j++)
            gLCDBuffer[i][j] = ' ';
}


/*
 

4-bit Initialization: 
// ********************************************************** 
void command(char i) 
{ 
    P1 = i;                       //put data on output Port 
    D_I =0;                       //D/I=LOW : send instruction 
    R_W =0;                       //R/W=LOW : Write     
    Nybble();                     //Send lower 4 bits
    i = i<<4;                     //Shift over by 4 bits 
    P1 = i;                       //put data on output Port 
    Nybble();                     //Send upper 4 bits 
} 

// ********************************************************** 
void write(char i) 
{ 
    P1 = i;                       //put data on output Port 
    D_I =1;                       //D/I=HIGH : send data 
    R_W =0;                       //R/W=LOW : Write    
    Nybble();                     //Clock lower 4 bits 
    i = i<<4;                     //Shift over by 4 bits 
    P1 = i;                       //put data on output Port 
    Nybble();                     //Clock upper 4 bits 
} 
 
// ********************************************************** 
void Nybble() 
{ 
    E = 1; 
    Delay(1);                     //enable pulse width >= 300ns 
    E = 0;                        //Clock enable: falling edge 
} 
 
// ********************************************************** 
void init() 
{ 
    P1 = 0; 
    P3 = 0; 
    Delay(100);                   //Wait >40 msec after power is applied 
    P1 = 0x30;                    //put 0x30 on the output port 
    Delay(30);                    //must wait 5ms, busy flag not available 
    Nybble();                     //command 0x30 = Wake up  
    Delay(10);                    //must wait 160us, busy flag not available 
    Nybble();                     //command 0x30 = Wake up #2 
    Delay(10);                    //must wait 160us, busy flag not available 
    Nybble();                     //command 0x30 = Wake up #3 
    Delay(10);                    //can check busy flag now instead of delay 
    P1= 0x20;                     //put 0x20 on the output port 
    Nybble();                     //Function set: 4-bit interface 
    command(0x28);                //Function set: 4-bit/2-line 
    command(0x10);                //Set cursor 
    command(0x0F);                //Display ON; Blinking cursor 
    command(0x06);                //Entry Mode set 
} 
// ********************************************************** 
 
 */

