/*
 * File:   lcd.c
 * Author: sm186105
 *
 * Created on July 11, 2017, 2:06 PM
 */


#include "xc.h"

#include "user.h"
#include "lcd.h"
#include "mcc_generated_files/tmr4.h"

// Declare the LCD buffer and round-robin index
char gLCDBuffer[16] = { };
char gLCDActual[16] = { 
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255 };
    
unsigned int gLCDIndex = 0;
bool gNeedToMove = false;
bool gIsCommand = false;
uint8_t gCommand = 0x80;
bool gSecondNybble = false;

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
    
    // Wait a minimum of 40ms after power-up before we do anything 
    __delay_ms(40);
    
    // Begin the wake-up sequence
    writeToLCDLAT(0x30, false);     // put 0x30 on the output port
    __delay_ms(5);                  // must wait 5ms busy flag not available
    NybbleSync();                   // command 0x30 = Wake up
    __delay_us(160);                // must wait 160us, busy flag not available 
    NybbleSync();                   // command 0x30 = Wake up #2 
    __delay_us(160);                // must wait 160us, busy flag not available 
    NybbleSync();                   // command 0x30 = Wake up #3 
    __delay_us(160);                // can check busy flag now instead of delay 
    writeToLCDLAT(0x20, false);     // put 0x20 on the output port 
    NybbleSync();                   // Function set: 4-bit interface 
    sendLCDCommand(0x28);           // Function set: 4-bit/2-line 
    sendLCDCommand(0x10);           // Set cursor 
    sendLCDCommand(0x0F);           // Display ON; Blinking cursor 
    sendLCDCommand(0x06);           // Entry Mode set 
    
    // Initialize the buffer
    writeLCDString(0, 0, "HUI ADDAver. 1.0");
    
    // Kick start the timer
    TMR4_Start();
}

void processLCDQueue()
{
    // Has enough time passed from the last LCD operation?
    if (TMR4_GetElapsedThenClear())
    {
        // Stop the timer
        TMR4_Stop();
        
        // Bring the enable back to low
        LATBbits.LATB14 = 0;
        
        // Perform the next LCD operation in the queue
        
        if (!gSecondNybble)
        {
            // Is the new character different than the previous one?
            if (!gIsCommand)
            {
                if (gLCDBuffer[gLCDIndex] == gLCDActual[gLCDIndex])
                {
                    // Move on to the next character
                    gLCDIndex = (gLCDIndex + 1) % 16;

                    // Mark that we'll need to move the cursor!
                    gNeedToMove = true;

                    // Re-start the timer and exit
                    TMR4_Start();
                    return;
                }
            }
            
            // Do we need to move first?
            if (gNeedToMove)
            {
                // Move the cursor to the new pos
                if (gLCDIndex < 8)
                    gCommand = 0x80 + gLCDIndex;
                else
                    gCommand = 0xC0 + gLCDIndex;
                
                // Setup the command sequence
                gIsCommand = true;
                gNeedToMove = false;
            }
            
            // Is it a command or data?
            if (gIsCommand)
            {
                // Output the upper half 
                writeToLCDLAT(gCommand, true);
                LATBbits.LATB15 = 0;    // D/I = low : send command
            }
            else
            {
                // Output the upper half 
                writeToLCDLAT(gLCDBuffer[gLCDIndex], true);
                LATBbits.LATB15 = 1;    // D/I = high : send data
            }
                
            LATBbits.LATB14 = 1;    // Enable -> high
            
            // Prepare for next cycle
            gSecondNybble = true;
        }
        else
        {
            if (gIsCommand)
            {
                // Output the lower half 
                writeToLCDLAT(gCommand, false);
                LATBbits.LATB15 = 0;    // D/I = low : send command
                
                // We are done with the command
                gIsCommand = false;
            }
            else
            {                
                // Output the lower half 
                writeToLCDLAT(gLCDBuffer[gLCDIndex], false);
                LATBbits.LATB15 = 1;    // D/I = high : send data
            }
            
            LATBbits.LATB14 = 1;    // Enable -> high
            
            // Record the new character
            gLCDActual[gLCDIndex] = gLCDBuffer[gLCDIndex];
            
            // Prepare for next cycle
            gLCDIndex = (gLCDIndex + 1) % 16;
            gSecondNybble = false;
        }
        
        // Start the timer again..
        TMR4_Start();
    }
}

void writeToLCDLAT(uint8_t b, bool bHighNibble)
{
    BitByte bb;
    bb.byte = b;
    
    // Are we writing out the lower or upper half of the byte?
    if (!bHighNibble)
    {
        LATBbits.LATB10 = bb.BITS.B0;
        LATBbits.LATB11 = bb.BITS.B1;
        LATBbits.LATB12 = bb.BITS.B2;
        LATBbits.LATB13 = bb.BITS.B3;
    }
    else
    {
        LATBbits.LATB10 = bb.BITS.B4;
        LATBbits.LATB11 = bb.BITS.B5;
        LATBbits.LATB12 = bb.BITS.B6;
        LATBbits.LATB13 = bb.BITS.B7;        
    }
}

void NybbleSync() 
{ 
    LATBbits.LATB14 = 1;        // Enable -> high
    __delay_us(1);              // enable pulse width >= 300ns
    LATBbits.LATB14 = 0;        // Clock enable: falling edge 
} 

void sendLCDCommand(char cCommand)
{
    writeToLCDLAT(cCommand, true);  // Send upper 4 bits first
    LATBbits.LATB15 = 0;            // D/I = low : send instruction
    NybbleSync();
    writeToLCDLAT(cCommand, false); // Send lower 4 bits
    NybbleSync();
    
    LATBbits.LATB15 = 1;            // Switch back to data mode
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
        unsigned int index = (r * 8) + c;
        gLCDBuffer[index] = ch;
        
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
    
    int i;
    for (i = 0; i < 16; i++)
        gLCDBuffer[i] = ' ';
    
    // Send the command to move cursor back to 0x00 position
    sendLCDCommand(0x80);
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

