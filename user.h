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

// Global declarations

// Some useful constants
#define _16BIT_1Q   0x3FFF
#define _16BIT_HALF 0x7FFF
#define _16BIT_3Q   0xBFFF
#define _16BIT_FS   0xFFFF

// The calibration coefficients in RAM
extern unsigned int gCoefficients[8][4];

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
        unsigned SD :1;
        unsigned OS :1;
        unsigned S1 :1;
        unsigned S0 :1;
        unsigned COM :1;
        unsigned UNI :1;
        unsigned SLP :1;    
        unsigned :9;
    } BITS;
    struct {
        uint16_t word;
    };
} LTC1867Config;
extern LTC1867Config gLTC1867Commands[8];

// Configuration 32bit sequence for the AD5668 DAC
typedef union tagAD5668Config {
    struct {
        unsigned :4;
        union {
            struct {
                unsigned C3 :1;
                unsigned C2 :1;
                unsigned C1 :1;
                unsigned C0 :1;
            } COMMAND_BITS;
            struct {
                unsigned command_value :4;
            };
        } command;
        union ADDRESS {
            struct {
                unsigned A3 :1;
                unsigned A2 :1;
                unsigned A1 :1;
                unsigned A0 :1;                
            } ADDRESS_BITS;
            struct {
                unsigned address_value :4;
            };
        } address;
        union {
            struct {
                unsigned D15 :1;
                unsigned D14 :1;
                unsigned D13 :1;
                unsigned D12 :1;
                unsigned D11 :1;
                unsigned D10 :1;
                unsigned D9 :1;
                unsigned D8 :1;
                unsigned D7 :1;
                unsigned D6 :1;
                unsigned D5 :1;
                unsigned D4 :1;
                unsigned D3 :1;
                unsigned D2 :1;
                unsigned D1 :1;
                unsigned D0 :1;
            } DATA_BITS;
            struct {
                uint16_t data;
            };
        };
        unsigned :3;
        unsigned DB0 :1;
    };
    struct {
        uint16_t words[2];
    };
} AD5668Config;

// Kick-start the A/D
void initializeADC();

// Process A/D jobs
void processADCPolling();
                
// Process transformations
void processChannel(int channel);
               
// Initialize the DAC internal reference
void initializeDAC();

// Process D/A jobs
void processDACUpdates();


#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* USER_H */

