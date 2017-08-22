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
uint16_t gCircularBuffers[8][NUM_OF_AVERAGES] = { };
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
    { .C0 = 0, .C1 = 0, .C2 = 0, .C3 = 1, .DB0 = 1 }
};

// The last D/A channel send to the DAC
unsigned int gLastDACChannel;

uint16_t ADCXChange(uint16_t *dataTransmitted, uint16_t byteCount, uint16_t *dataReceived)
{
    uint16_t retval = 0;
    
    LATBbits.LATB0 = 0;
    retval = SPI1_Exchange16bitBuffer(dataTransmitted, byteCount, dataReceived);
    __delay_us(1);
    LATBbits.LATB0 = 1;
    
    return retval;
}

uint16_t DACXChange(AD5668Config adVariable, uint16_t *dataReceived)
{
    uint16_t retval = 0;
    
    uint16_t wh = adVariable.words[1];
    uint16_t wl = adVariable.words[0];

    LATBbits.LATB6 = 0;
    retval = SPI2_Exchange16bitBuffer(&wh, 2, dataReceived);
    retval = SPI2_Exchange16bitBuffer(&wl, 2, dataReceived);
    __delay_us(8);
    LATBbits.LATB6 = 1;
    
    return retval;
}

// Kick-start the A/D
void initializeADC()
{
    // Send command to LTC to begin sampling CH0
    uint16_t receiveBuffer; // the dummy receive buffer
    ADCXChange(&gLTC1867Commands[0].word, 2, &receiveBuffer);

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
        ADCXChange(&gLTC1867Commands[nextChannel].word, 2, &gIncomingValues[gLastADChannel]);

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
    ADCXChange(&gLTC1867Commands[channel].word, 2, &buffer);

    __delay_us(10);
    
    // And this call to actually read back the value!
    ADCXChange(&gLTC1867Commands[channel].word, 2, &buffer);

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
    DACXChange(gAD5668EnableIntRef, NULL);
    
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
    //DACConfig.command.command_value = 0x3;  // Write to and update DAC channel n
    DACConfig.C1 = 1;
    DACConfig.C0 = 1;
    Address4Bit addr;
    addr.address_value = channel;
    DACConfig.A0 = addr.ADDRESS_BITS.A0;
    DACConfig.A1 = addr.ADDRESS_BITS.A1;
    DACConfig.A2 = addr.ADDRESS_BITS.A2;
    DACConfig.A3 = addr.ADDRESS_BITS.A3;
    
    // Setup the output value
    //DACConfig.data.data_value = gOutgoingValues[channel];
    Data16Bit data;
    data.data_value = gOutgoingValues[channel];
    DACConfig.D0 = data.DATA_BITS.D0;
    DACConfig.D1 = data.DATA_BITS.D1;
    DACConfig.D2 = data.DATA_BITS.D2;
    DACConfig.D3 = data.DATA_BITS.D3;
    DACConfig.D4 = data.DATA_BITS.D4;
    DACConfig.D5 = data.DATA_BITS.D5;
    DACConfig.D6 = data.DATA_BITS.D6;
    DACConfig.D7 = data.DATA_BITS.D7;
    DACConfig.D8 = data.DATA_BITS.D8;
    DACConfig.D9 = data.DATA_BITS.D9;
    DACConfig.D10 = data.DATA_BITS.D10;
    DACConfig.D11 = data.DATA_BITS.D11;
    DACConfig.D12 = data.DATA_BITS.D12;
    DACConfig.D13 = data.DATA_BITS.D13;
    DACConfig.D14 = data.DATA_BITS.D14;
    DACConfig.D15 = data.DATA_BITS.D15;
    
    // Initiate the SPI communication to the DAC
    DACXChange(DACConfig, NULL);
    
    // Update the last channel indicator
    gLastDACChannel = channel;
}
