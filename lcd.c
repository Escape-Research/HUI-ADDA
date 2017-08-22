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

// Global LCD on/off switch
bool gLCDon = true;

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
    writeToLCDLAT(0x30, true);      // put 0x30 on the output port
    __delay_ms(5);                  // must wait 5ms busy flag not available
    NybbleSync();                   // command 0x30 = Wake up
    __delay_us(160);                // must wait 160us, busy flag not available 
    NybbleSync();                   // command 0x30 = Wake up #2 
    __delay_us(160);                // must wait 160us, busy flag not available 
    NybbleSync();                   // command 0x30 = Wake up #3 
    __delay_us(160);                // can check busy flag now instead of delay 
    writeToLCDLAT(0x20, true);      // put 0x20 on the output port 
    NybbleSync();                   // Function set: 4-bit interface 

    sendLCDCommand(0x28);           // Function set: 4-bit/2-line 
    
    // Write special symbols to CGRAM
    // TODO
    
    // Prepare display for first use
    sendLCDCommand(0x10);           // Set cursor 
    sendLCDCommand(0x0F);           // Display ON; Blinking cursor 
    sendLCDCommand(0x06);           // Entry Mode set 
    
    // Initialize the buffer
    writeLCDStringSync(0, 0, "HUI ADDAver. 1.0");

    // Wait for 1 sec (to read version on display)
    __delay_ms(100);
    
    // Kick start the timer
    TMR4_Start();
}

void switchLCDState(bool bOnOff)
{
    // Is there anything to do?
    if (bOnOff == gLCDon)
        return;
    
    if (bOnOff)
    {
        // Turn off the LCD

        TMR4_Stop();    // Stop the LCD asynchronous updating
        
        // Clear the screen
        writeLCDStringSync(0, 0, "                ");
        // Move cursor back to 0x00 position
        sendLCDCommand(0x80);
        
        // Set the global flag
        gLCDon = false;
    }
    else
    {
        // Set the global flag
        gLCDon = true;
        
        // Initialize the buffer
        writeLCDString(0, 0, "HUI ADDAver. 1.0");
        
        // Kick start the timer
        TMR4_Start();        
    }
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
                    // Mark that we'll need to move the cursor!
                    gNeedToMove = true;

                    // Move on to the next character
                    gLCDIndex = (gLCDIndex + 1) % 16;

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
                    gCommand = 0xC0 + (gLCDIndex - 8);
                
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
                
                // Record the new character
                gLCDActual[gLCDIndex] = gLCDBuffer[gLCDIndex];

                // Was this the last character?
                //if (gLCDIndex == 15)
                    // Send the command to move cursor back to 0x00 position
                //    sendLCDCommand(0x80);

                // Prepare for next cycle
                gLCDIndex = (gLCDIndex + 1) % 16;               
            }
            
            LATBbits.LATB14 = 1;    // Enable -> high
            
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
    
    // Is the LCD on?
    if (!gLCDon)
        return;
    
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
    // Is the LCD on?
    if (!gLCDon)
        return;

    LATBbits.LATB14 = 1;        // Enable -> high
    __delay_us(40);             // enable pulse width >= 300ns
    LATBbits.LATB14 = 0;        // Clock enable: falling edge 
} 

void sendLCDCommand(char cCommand)
{
    // Is the LCD on?
    if (!gLCDon)
        return;

    writeToLCDLAT(cCommand, true);  // Send upper 4 bits first
    LATBbits.LATB15 = 0;            // D/I = low : send instruction
    NybbleSync();
    writeToLCDLAT(cCommand, false); // Send lower 4 bits
    NybbleSync();
    
    LATBbits.LATB15 = 1;            // Switch back to data mode
}

void sendLCDData(char cData)
{
    // Is the LCD on?
    if (!gLCDon)
        return;

    writeToLCDLAT(cData, true);     // Send upper 4 bits first
    LATBbits.LATB15 = 1;            // D/I = high : send data
    NybbleSync();
    writeToLCDLAT(cData, false);    // Send lower 4 bits
    NybbleSync();
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

void writeLCDStringSync(unsigned int row, unsigned int column, char *pString)
{
    // Update the LCD buffer
    writeLCDString(row, column, pString);
    
    // Is the LCD on?
    if (!gLCDon)
        return;
    
    // Make sure that Timer 4 will not interrupt this call
    TMR4_Stop();
    
    // Send the command to move cursor back to 0x00 position
    sendLCDCommand(0x80);
    
    // Write the first row on the display
    int index;
    for (index = 0; index < 16; index++)
    {
        // Do we need to move to the next line?
        if (index == 8)
            // Move the DDRAM location to 0x40
            sendLCDCommand(0xC0);    
        
        // Send character to LCD
        sendLCDData(gLCDBuffer[index]);
        // Update the "actual" buffer contents
        gLCDActual[index] = gLCDBuffer[index];
    }
    
    // Re-start the timer
    TMR4_Start();
}

void clearLCDScreen()
{
    // Fill the buffer with spaces
    
    int i;
    for (i = 0; i < 16; i++)
        gLCDBuffer[i] = ' ';
    
    // Send the command to clear screen
    sendLCDCommand(0x01);
}
