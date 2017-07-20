/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

#ifndef LCD_H
#define	LCD_H

#include <xc.h> // include processor files - each processor file is guarded.  

// Declare the LCD buffer and round-robin index
extern char gLCDBuffer[16];
extern char gLCDActual[16]; 
extern unsigned int gLCDIndex;
extern bool gSecondNybble;


typedef union tagBitByte {
    struct {
        unsigned B0 :1;
        unsigned B1 :1;
        unsigned B2 :1;
        unsigned B3 :1;
        unsigned B4 :1;
        unsigned B5 :1;
        unsigned B6 :1;    
        unsigned B7 :1;
    } BITS;
    struct {
        uint8_t byte;
    };
} BitByte;

// public methods
void initializeLCD();
void processLCDQueue();
void writeLCDString(unsigned int row, unsigned int column, char *pString);
void clearLCDScreen();

// private methods
void writeToLCDLAT(uint8_t b, bool bHighNibble);
void NybbleSync(); 
void sendLCDCommand(char cCommand);


#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* LCD_H */

