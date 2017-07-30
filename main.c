/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using MPLAB(c) Code Configurator

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  MPLAB(c) Code Configurator - pic24-dspic-pic32mm : v1.26
        Device            :  dsPIC33EV256GM102
    The generated drivers are tested against the following:
        Compiler          :  XC16 1.30
        MPLAB             :  MPLAB X 3.45
*/

/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

#include "mcc_generated_files/mcc.h"

// Include the common definitions for this project
#include "user.h"
#include "flash.h"
#include "lcd.h"

// Declaration of global variables
RUN_STATE gCurrentState = init; 
uint8_t gCalState = 0;

// Channel buffer for capturing and calculating averages
uint16_t gAvgBuffer[8][16] = { };

// Clear the buffer used for averaging
void clearAvgBuffer()
{
    int i, j;
    for (i = 0; i < 8; i++)
        for (j = 0; j < 16; j++)
            gAvgBuffer[i][j] = 0;
}

/*
                         Main application
 */
int main(void)
{
    // initialize the device
    SYSTEM_Initialize();

    // When using interrupts, you need to set the Global Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
    //INTERRUPT_GlobalEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalDisable();

    // Simple counters used to detect (and de-bounce push buttons)
    unsigned int nBtn1Counter = 0;
    unsigned int nBtn2Counter = 0;
    
    unsigned int currCalibrationChannel = 0;
    unsigned int currCalibrationCount = 0;
    
    uint16_t value = 0;
    
    // Simple state machine
    while (1)
    {
        switch (gCurrentState)
        {
            case init:
                
                // Initialize the LCD
                initializeLCD();
                
                // Kick start the A/D
                initializeADC();
                
                // Initialize the D/A internal reference
                initializeDAC();
                
                // Load coefficients from flash
                readFromFlash((unsigned int *)gCoefficients, 32);
                
                // Update the LCD
                writeLCDStringSync(0, 0, "********12345678");
                
                // switch to normal mode
                gCurrentState = normal;
                
                break;
                
            case normal:
                
                // Process calibration button
                if (!PORTAbits.RA0)
                    nBtn1Counter++;
                else
                {
                    // Should we enter calibration mode?
                    if (nBtn1Counter > 1000)
                    {
                        gCurrentState = calibration;
                        gCalState = 0;
                        nBtn1Counter = 0;
                        currCalibrationChannel = 0;
                        currCalibrationCount = 0;
                        
                        // Clear the avg. buffer
                        clearAvgBuffer();

                        // Update the LCD (we start from the 25% point)
                        writeLCDStringSync(0, 0, "--------Cal: 25%");
                    }
                }
                
                
                // Process LCD on/off button
                if (!PORTAbits.RA1)
                    nBtn2Counter++;
                else
                {
                    // Should we change the LCD state?
                    if (nBtn2Counter > 100)
                    {
                         // Update the LCD based on the new state
                        if (gLCDon)
                        {
                           writeLCDStringSync(0, 0, "                ");
                           switchLCDState(false);
                        }
                        else
                        {
                           switchLCDState(true);
                           writeLCDStringSync(0, 0, "********12345678");                       
                        }
                    }
                    nBtn2Counter = 0;
                }
                        
                // We are implementing a form of "non preemptive" multi-tasking
                
                // Process A/D and transformation jobs
                processADCPolling();
                
                // Process D/A jobs
                processDACUpdates();
                
                // Process LCD updating
                processLCDQueue();
                
                break;
                
            case calibration:
                
                // Do calibration
                
                // Round-robin the channels and buffer positions
                
                // read the next buffer location for the curr. channel
                value = readADCChannel(currCalibrationChannel);
                gAvgBuffer[currCalibrationChannel][currCalibrationCount] = value;
                
                // Do we have 1t6 readings for this channel?
                currCalibrationCount++;
                if (currCalibrationCount > 15)
                {
                    // Calculate and update the coefficient for this channel
                    double avg = 0;
                    int i = 0;
                    for (i = 0; i < 16; i++)
                        avg += gAvgBuffer[currCalibrationChannel][i];
                    avg /= 16;
                    
                    // Update the coefficient for this position and channel
                    gCoefficients[currCalibrationChannel][gCalState] = avg;
                    
                    // Update the display
                    writeLCDString(0, currCalibrationChannel, "+");
                    
                    // round-robin the channels
                    currCalibrationChannel = (currCalibrationChannel + 1) % 8;
                    
                    // reset the counter
                    currCalibrationCount = 0;
                }
                
                // Process calibration mode button
                if (!PORTAbits.RA1)
                    nBtn2Counter++;
                else
                {
                    // Should we switch the calibration mode?
                    if (nBtn2Counter > 100)
                    {
                        // Switch the calibration mode
                        gCalState = (gCalState + 1) % 4;
                        nBtn2Counter = 0;
                        
                        // Update the display based on cal. state
                        switch (gCalState)
                        {
                            case 0:     // first quarter
                                writeLCDStringSync(1, 0, "Cal: 25%");
                                break;
                            case 1:     // half
                                writeLCDStringSync(1, 0, "Cal: 50%");
                                break;
                            case 2:     // 3rd quarter
                                writeLCDStringSync(1, 0, "Cal: 75%");
                                break;
                            case 3:     // full scale
                                writeLCDStringSync(1, 0, "Cal:100%");
                                break;
                        }
                        
                        // Clear the avg. buffer
                        clearAvgBuffer();
                    }
                    nBtn2Counter = 0;
                }
 
                // Process calibration exit button
                if (!PORTAbits.RA0)
                    nBtn1Counter++;
                else
                {
                    // Should we exit the calibration mode?
                    if (nBtn1Counter > 1000)
                    {
                        // Reset transient state variables
                        gCurrentState = normal;
                        gCalState = 0;
                        nBtn1Counter = 0;
                        nBtn2Counter = 0;

                        // Update the LCD
                        writeLCDStringSync(1, 0, "Saving..");

                        // Save coefficients in flash
                        eraseFlashStorage();
                        writeToFlash((unsigned int *)gCoefficients, 32);

                        // Update the LCD
                        writeLCDStringSync(0, 0, "********12345678");                
                    }
                    nBtn1Counter = 0;
                }
                
                // Process LCD updates
                processLCDQueue();
        }
    }
    
    return 0;
}
/**
 End of File
*/