/*
 * File:   user.c
 * Author: sm186105
 *
 * Created on July 6, 2017, 1:14 PM
 */


#include "xc.h"
#include "user.h"

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

// Process A/D jobs
void processADPolling()
{
    
}
                
// Process transformations
void processTXJobs()
{
    
}
                
// Process D/A jobs
void processDAUpdates()
{
    
}

