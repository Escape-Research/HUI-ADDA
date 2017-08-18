/*
 * File:   user.c
 * Author: sm186105
 *
 * Created on July 6, 2017, 1:14 PM
 */

// Environment includes
#include "xc.h"
#include "stdlib.h"

// Project includes
#include "user.h"
#include "lcd.h"
#include "mcc_generated_files/tmr2.h"
#include "mcc_generated_files/spi1.h"
#include "mcc_generated_files/spi2.h"

// The calibration coefficients in RAM
unsigned int gCoefficients[8][4] = { 
    { _16BIT_1Q, _16BIT_HALF, _16BIT_3Q, _16BIT_FS },
    { _16BIT_1Q, _16BIT_HALF, _16BIT_3Q, _16BIT_FS },
    { _16BIT_1Q, _16BIT_HALF, _16BIT_3Q, _16BIT_FS },
    { _16BIT_1Q, _16BIT_HALF, _16BIT_3Q, _16BIT_FS },
    { _16BIT_1Q, _16BIT_HALF, _16BIT_3Q, _16BIT_FS },
    { _16BIT_1Q, _16BIT_HALF, _16BIT_3Q, _16BIT_FS },
    { _16BIT_1Q, _16BIT_HALF, _16BIT_3Q, _16BIT_FS },
    { _16BIT_1Q, _16BIT_HALF, _16BIT_3Q, _16BIT_FS }
};

// Averaging Buffers (for noise suppression)
bool gUseAveraging = true;
uint16_t gCircularBuffers[8][16] = { };
unsigned int gCircularBufferHead[8] = { };

// The LTC1867 sequential commands
LTC1867Config gLTC1867Commands[8] = {
    { .BITS = { 0, 1, 0, 0, 0, 0, 1 }},    // CHO
    { .BITS = { 0, 1, 0, 0, 0, 1, 1 }},    // CH1
    { .BITS = { 0, 1, 0, 1, 0, 0, 1 }},    // CH2
    { .BITS = { 0, 1, 0, 1, 0, 1, 1 }},    // CH3
    { .BITS = { 0, 1, 0, 0, 1, 0, 1 }},    // CH4
    { .BITS = { 0, 1, 0, 0, 1, 1, 1 }},    // CH5
    { .BITS = { 0, 1, 0, 1, 1, 0, 1 }},    // CH6
    { .BITS = { 0, 1, 0, 1, 1, 1, 1 }}     // CH7
};

// The last A/D channel requested
unsigned int gLastADChannel;

// The "incoming" A/D values
uint16_t gIncomingValues[8] = { };

// The "outgoing" or processed values for the D/A
uint16_t gOutgoingValues[8] = { };

// The 32bit configuration to turn on the internal reference for AD5668
AD5668Config gAD5668EnableIntRef = {
    { .command.COMMAND_BITS = { 1, 0, 0, 0 }, .DB0 = 1 }
};

// The last D/A channel send to the DAC
unsigned int gLastDACChannel;


// Kick-start the A/D
void initializeADC()
{
    // Send command to LTC to begin sampling CH0
    uint16_t receiveBuffer; // the dummy receive buffer
    //INTERRUPT_GlobalDisable();
    SPI1_Exchange16bitBuffer(&gLTC1867Commands[0].word, 2, &receiveBuffer);
    //INTERRUPT_GlobalEnable();

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
        //INTERRUPT_GlobalDisable();
        SPI1_Exchange16bitBuffer(&gLTC1867Commands[nextChannel].word, 2, &gIncomingValues[gLastADChannel]);
        //INTERRUPT_GlobalEnable();

        // Start timer 2 to indicate when the A/D will have finished the conversion
        TMR2_Start();
        
        // Capture the value "before"
        uint16_t prevValue = gOutgoingValues[gLastADChannel];
        
        // Call up the transformation function
        processChannel(gLastADChannel);
        
        // Did the value change?
        bool bChange = (prevValue != gOutgoingValues[gLastADChannel]);
        
        // Update the LCD indicator for that channel
        writeLCDString(0, gLastADChannel, (bChange)? "*" : "-");
/*        if (gLastADChannel == 0)
        {
            char buffer[8] = { };
            writeLCDString(0, 0, itoa(buffer, gOutgoingValues[gLastADChannel], 16));
        }
*/
        
        // Update the last requested channel indicator
        gLastADChannel = nextChannel;
    }
}

// Perform a single ADC read
uint16_t readADCChannel(unsigned int channel)
{   
    // Send / Receive on the A/D SPI
    uint16_t buffer = 0;
    
    // Use this call to tell the ADC which channel we want to read
    //INTERRUPT_GlobalDisable();
    SPI1_Exchange16bitBuffer(&gLTC1867Commands[channel].word, 2, &buffer);
    __delay_us(10);
    
    // And this call to actually read back the value!
    SPI1_Exchange16bitBuffer(&gLTC1867Commands[channel].word, 2, &buffer);
    //INTERRUPT_GlobalEnable();

    __delay_us(10);
    
    return buffer;
}

// Push a new value into the circular buffer (used for averaging)
void pushToBuffer(unsigned int nBuffer, uint16_t value)
{
    // Store the next value and update the index
    gCircularBuffers[nBuffer][gCircularBufferHead[nBuffer]] = value;
    
    // Update the index
    gCircularBufferHead[nBuffer] = (gCircularBufferHead[nBuffer] + 1) % NUM_OF_AVERAGES;
}

// Calculate and return the current average value for a channel 
// using the last stored values in the circular buffers
uint16_t getAvgForChannel(unsigned int nBuffer)
{
    // Calculate the average value for a channel
    double dTotal = 0;
    
    int i;
    for (i = 0; i < NUM_OF_AVERAGES; i++)
        dTotal += gCircularBuffers[nBuffer][i];
    
    dTotal /= NUM_OF_AVERAGES;
    
    return dTotal;
}
            
// Process transformations
void processChannel(int channel)
{
    // The incoming unprocessed value
    uint16_t input = gIncomingValues[channel];
    
    // Are we using averaging?
    if (gUseAveraging)
    {
        // Push the last value into the circular buffer
        pushToBuffer(channel, input);
        
        // ... and calculate the average value for this channel
        input = getAvgForChannel(channel);
    }
    
    double factor;
    uint16_t result = 0;
    
    // Figure out which region we're in, calculate and correct as needed
    
    // Lower boundary
    if (input == 0)
    {
        gOutgoingValues[channel] = 0;
        return;
    }
    
    // Between lower boundary and 1st quarter
    if (input < gCoefficients[channel][0])
    {
        factor = input * (double)(_16BIT_1Q);
        factor /= (double)(gCoefficients[channel][0]);
        result = factor;
    }
    
    // Exactly at 1st quarter
    else if (input == gCoefficients[channel][0])
    {
        gOutgoingValues[channel] = _16BIT_1Q;
        return;
    }
    
    // Between 1st quarter and halfway through
    else if (input < gCoefficients[channel][1])
    {
        factor = ( input - gCoefficients[channel][0] ) * (double)(_16BIT_1Q);
        factor /= (double)(gCoefficients[channel][1] - gCoefficients[channel][0]);
        result = factor + _16BIT_1Q;
    }
    
    // Exactly halfway through
    else if (input == gCoefficients[channel][1])
    {        
        gOutgoingValues[channel] = _16BIT_HALF;
        return;
    }
    
    // Between halfway through and 3rd quarter
    else if (input < gCoefficients[channel][2])
    {
        factor = ( input - gCoefficients[channel][1] ) * (double)(_16BIT_1Q);
        factor /= (double)(gCoefficients[channel][2] - gCoefficients[channel][1]);        
        result = factor + _16BIT_HALF;
    }
    
    // Exactly at 3rd quarter
    else if (input == gCoefficients[channel][2])
    {
        gOutgoingValues[channel] = _16BIT_3Q;
        return;
    }
    
    // Between 3rd quarter and full scale
    else if (input < gCoefficients[channel][3])
    {
        factor = ( input - gCoefficients[channel][2] ) * (double)(_16BIT_1Q);
        factor /= (double)(gCoefficients[channel][3] - gCoefficients[channel][2]);        
        result = factor + _16BIT_3Q;
    }
    
    // Exactly at full scale
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
    //uint16_t receiveBuffer[2]; // dummy receive buffer
    //INTERRUPT_GlobalDisable();
    SPI2_Exchange16bitBuffer(gAD5668EnableIntRef.words, 4, NULL);
    //INTERRUPT_GlobalEnable();
    
    // Setup last channel indicator to the last channel (so it will roll to first)
    gLastDACChannel = 7;
}

// Process D/A jobs
void processDACUpdates()
{
    // Round-robin the output channel
    unsigned int channel = (gLastDACChannel + 1) % 8;
 
    // Setup the configuration 32 bit structure
    AD5668Config DACConfig = { };
    
    // Setup the command and DAC channel
    DACConfig.command.command_value = 0x3;  // Write to and update DAC channel n
    DACConfig.address.address_value = channel;
    
    // Setup the output value
    DACConfig.data = gOutgoingValues[channel];
    
    // Initiate the SPI communication to the DAC
    //uint16_t receiveBuffer[2]; // dummy receive buffer
    //INTERRUPT_GlobalDisable();
    SPI2_Exchange16bitBuffer(DACConfig.words, 4, NULL);
    //INTERRUPT_GlobalEnable();
    
    // Update the last channel indicator
    gLastDACChannel = channel;
}
