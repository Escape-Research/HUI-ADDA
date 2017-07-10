/*
 * File:   user.c
 * Author: sm186105
 *
 * Created on July 6, 2017, 1:14 PM
 */


#include "xc.h"

#include "user.h"
#include "mcc_generated_files/tmr2.h"

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

// Kick-start the A/D
void initializeAD()
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
void processADPolling()
{
    // Is the last conversion done?
    if (TMR2_GetElapsedThenClear())
    {
        // round-robin for the A/D channels
        unsigned int nextChannel = (gLastADChannel + 1) % 8;
        
        // Send / Receive on the A/D SPI
        SPI1_Exchange16bitBuffer(&gLTC1867Commands[nextChannel].word, 2, &gIncomingValues[gLastADChannel]);

        // Start timer 2 to indicate when the A/D has finished the conversion
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
    
}

/*
// Return the "calibrated" 10bit value for a fader
uint16_t map_location(int fader, uint16_t fpos)
{
    double factor;
    uint16_t result = 0;
    
    // Figure out which region we're in
    if (fpos == 0)
    {
        return 0;
    }
    if (fpos < map_cal[fader][1])
    {
        factor = fpos * (double)(_12BIT_1Q);
        factor /= (double)(map_cal[fader][1]);
        result = factor;
    }
    else if (fpos == map_cal[fader][1])
    {
        return _10BIT_1Q;
    }
    else if (fpos < map_cal[fader][0])
    {
        factor = ( fpos - map_cal[fader][1] ) * (double)(_12BIT_1Q);
        factor /= (double)(map_cal[fader][0] - map_cal[fader][1]);
        result = factor + _12BIT_1Q;
    }
    else if (fpos == map_cal[fader][0])
    {
        return _10BIT_HALF;
    }
    else if (fpos < map_cal[fader][2])
    {
        factor = ( fpos - map_cal[fader][0] ) * (double)(_12BIT_1Q);
        factor /= (double)(map_cal[fader][2] - map_cal[fader][0]);        
        result = factor + _12BIT_HALF;
    }
    else if (fpos == map_cal[fader][2])
    {
        return _10BIT_3Q;
    }
    else if (fpos < map_cal[fader][3])
    {
        factor = ( fpos - map_cal[fader][2] ) * (double)(_12BIT_1Q);
        factor /= (double)(map_cal[fader][3] - map_cal[fader][2]);        
        result = factor + _12BIT_3Q;
    }
    else
        return _10BIT_FS;
    
    result = scale_from_12_to_10bits(result);
    return result;
}
*/
                
// Process D/A jobs
void processDAUpdates()
{
    
}

