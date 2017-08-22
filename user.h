/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

#ifndef USER_H
#define	USER_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>

// Declare the __delay functions
#include "mcc_generated_files/mcc.h"
#define FCY (_XTAL_FREQ / 2)
#include "libpic30.h"

// Global declarations

// Some useful constants
#define _16BIT_1Q   0x3FFF
#define _16BIT_HALF 0x7FFF
#define _16BIT_3Q   0xBFFF
#define _16BIT_FS   0xFFFF

// The calibration coefficients in RAM
extern unsigned int gCoefficients[8][4];

// Averaging Buffers (for noise suppression)
#define NUM_OF_AVERAGES 4
extern bool gUseAveraging;
extern uint16_t gCircularBuffers[8][NUM_OF_AVERAGES];
extern unsigned int gCircularBufferHead[8];

// Machine States
typedef enum tagRUN_STATE {
    init        = 0,
    normal      = 1,
    calibration = 2
} RUN_STATE;
extern RUN_STATE gCurrentState;

// Configuration 16bit word for the LTC1867 ADC
typedef union tagLTC1867Config {
    struct {
        unsigned :9;
        unsigned SLP :1;    
        unsigned UNI :1;
        unsigned COM :1;
        unsigned S0 :1;
        unsigned S1 :1;
        unsigned OS :1;        
        unsigned SD :1;
    } BITS;
    struct {
        uint16_t word;
    };
} LTC1867Config;
extern LTC1867Config gLTC1867Commands[8];

typedef union tag16bitValue {
    struct {
        unsigned D0 :1;
        unsigned D1 :1;
        unsigned D2 :1;
        unsigned D3 :1;
        unsigned D4 :1;
        unsigned D5 :1;
        unsigned D6 :1;
        unsigned D7 :1;
        unsigned D8 :1;
        unsigned D9 :1;
        unsigned D10 :1;
        unsigned D11 :1;
        unsigned D12 :1;
        unsigned D13 :1;
        unsigned D14 :1;
        unsigned D15 :1;
    } DATA_BITS;
    uint16_t data_value;
} Data16Bit;

typedef union tag4bitAddress {
    struct {
        unsigned A0 :1;
        unsigned A1 :1;
        unsigned A2 :1;
        unsigned A3 :1;                
    } ADDRESS_BITS;
    struct {
        unsigned address_value :4;
    };
} Address4Bit;

typedef union tag4bitCommand {
    struct {
        unsigned C0 :1;
        unsigned C1 :1;
        unsigned C2 :1;
        unsigned C3 :1;
    } COMMAND_BITS;
    struct {
        unsigned command_value :4;
    };
} Command4Bit;

// Configuration 32bit sequence for the AD5668 DAC
typedef union tagAD5668Config {
    struct {
        unsigned DB0 :1;
        unsigned :3;
        unsigned D0 :1;
        unsigned D1 :1;
        unsigned D2 :1;
        unsigned D3 :1;
        unsigned D4 :1;
        unsigned D5 :1;
        unsigned D6 :1;
        unsigned D7 :1;
        unsigned D8 :1;
        unsigned D9 :1;
        unsigned D10 :1;
        unsigned D11 :1;
        unsigned D12 :1;
        unsigned D13 :1;
        unsigned D14 :1;
        unsigned D15 :1;
        unsigned A0 :1;
        unsigned A1 :1;
        unsigned A2 :1;
        unsigned A3 :1;                
        unsigned C0 :1;
        unsigned C1 :1;
        unsigned C2 :1;
        unsigned C3 :1;
        unsigned :4;
    };
    uint16_t words[2];
} AD5668Config;

// Kick-start the A/D
void initializeADC();

// ADC communication routine
uint16_t ADCXChange(uint16_t *dataTransmitted, uint16_t byteCount, uint16_t *dataReceived);

// Process A/D jobs
void processADCPolling();

// Perform a single ADC read
uint16_t readADCChannel(unsigned int channel);  
                
// Process transformations
void processChannel(int channel);
               
// Initialize the DAC internal reference
void initializeDAC();

// DAC communication routine
uint16_t DACXChange(AD5668Config adVariable, uint16_t *dataReceived);

// Process D/A jobs
void processDACUpdates();

// Circular buffer management and averaging
void pushToBuffer(unsigned int nBuffer, uint16_t value);
uint16_t getAvgForChannel(unsigned int nBuffer);


#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* USER_H */

