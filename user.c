/*
 * File:   user.c
 * Author: sm186105
 *
 * Created on July 6, 2017, 1:14 PM
 */

// Environment includes
#include "xc.h"

// Project includes
#include "user.h"
#include "mcc_generated_files/tmr2.h"
#include "mcc_generated_files/spi1.h"
#include "mcc_generated_files/spi2.h"

// The calibration coefficients in RAM
unsigned int gCoefficients[8][4] = { };

// The LTC1867 sequential commands
LTC1867Config gLTC1867Commands[8] = {
    { .BITS = { 1, 0, 0, 0, 0, 1, 0 }},    // CHO
    { .BITS = { 1, 1, 0, 0, 0, 1, 0 }},    // CH1
    { .BITS = { 1, 0, 0, 1, 0, 1, 0 }},    // CH2
    { .BITS = { 1, 1, 0, 1, 0, 1, 0 }},    // CH3
    { .BITS = { 1, 0, 1, 0, 0, 1, 0 }},    // CH4
    { .BITS = { 1, 1, 1, 0, 0, 1, 0 }},    // CH5
    { .BITS = { 1, 0, 1, 1, 0, 1, 0 }},    // CH6
    { .BITS = { 1, 1, 1, 1, 0, 1, 0 }}     // CH7
};

// The last A/D channel requested
unsigned int gLastADChannel;

// The "incoming" A/D values
uint16_t gIncomingValues[8];

// The "outgoing" or processed values for the D/A
uint16_t gOutgoingValues[8];

// The 32bit configuration to turn on the internal reference for AD5668
AD5668Config gAD5668EnableIntRef = {
    { .command.COMMAND_BITS = { 1, 0, 0, 0 }, .DB0 = 1 }
};

// Kick-start the A/D
void initializeADC()
{
    // Send command to LTC to begin sampling CH0
    uint16_t receiveBuffer; // the dummy receive buffer
    SPI1_Exchange16bitBuffer(&gLTC1867Commands[0].word, 2, &receiveBuffer);
    
    // Update the last requested channel indicator
    gLastADChannel = 0;
    
    // Start timer 2 to indicate when the A/D has finished the conversion
    TMR2_Start();
}

// Process A/D jobs
void processADCPolling()
{
    // Is the last conversion done?
    if (TMR2_GetElapsedThenClear())
    {
        // Stop the timer
        TMR2_Stop();
        
        // round-robin for the A/D channels
        unsigned int nextChannel = (gLastADChannel + 1) % 8;
        
        // Send / Receive on the A/D SPI
        SPI1_Exchange16bitBuffer(&gLTC1867Commands[nextChannel].word, 2, &gIncomingValues[gLastADChannel]);

        // Start timer 2 to indicate when the A/D will have finished the conversion
        TMR2_Start();
        
        // Call up the transformation function
        processChannel(gLastADChannel);

        // Update the last requested channel indicator
        gLastADChannel = nextChannel;
    }
}
                
// Process transformations
void processChannel(int channel)
{
    // The incoming unprocessed value
    uint16_t input = gIncomingValues[channel];
    
    double factor;
    uint16_t result = 0;
    
    // Figure out which region we're in
    if (input == 0)
    {
        gOutgoingValues[channel] = 0;
        return;
    }
    if (input < gCoefficients[channel][1])
    {
        factor = input * (double)(_16BIT_1Q);
        factor /= (double)(gCoefficients[channel][1]);
        result = factor;
    }
    else if (input == gCoefficients[channel][1])
    {
        gOutgoingValues[channel] = _16BIT_1Q;
        return;
    }
    else if (input < gCoefficients[channel][0])
    {
        factor = ( input - gCoefficients[channel][1] ) * (double)(_16BIT_1Q);
        factor /= (double)(gCoefficients[channel][0] - gCoefficients[channel][1]);
        result = factor + _16BIT_1Q;
    }
    else if (input == gCoefficients[channel][0])
    {        
        gOutgoingValues[channel] = _16BIT_HALF;
        return;
    }
    else if (input < gCoefficients[channel][2])
    {
        factor = ( input - gCoefficients[channel][0] ) * (double)(_16BIT_1Q);
        factor /= (double)(gCoefficients[channel][2] - gCoefficients[channel][0]);        
        result = factor + _16BIT_HALF;
    }
    else if (input == gCoefficients[channel][2])
    {
        gOutgoingValues[channel] = _16BIT_3Q;
        return;
    }
    else if (input < gCoefficients[channel][3])
    {
        factor = ( input - gCoefficients[channel][2] ) * (double)(_16BIT_1Q);
        factor /= (double)(gCoefficients[channel][3] - gCoefficients[channel][2]);        
        result = factor + _16BIT_3Q;
    }
    else
    {
        gOutgoingValues[channel] = _16BIT_FS;
        return;
    }

    // Save the processed value 
    gOutgoingValues[channel] = result;
}
  
// Setup the internal reference for the AD5668 DAC
void initializeDAC()
{
    uint16_t receiveBuffer[2]; // the dummy receive buffer
    SPI2_Exchange16bitBuffer(gAD5668EnableIntRef.words, 4, receiveBuffer);
}

// Process D/A jobs
void processDACUpdates()
{
    
}

