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
                writeLCDString(0, 0, "********12345678");
                
                // switch to normal mode
                gCurrentState = normal;
                
                break;
                
            case normal:
                
                // Process calibration button
                if (PORTAbits.RA0)
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

                        // Update the LCD
                        writeLCDString(0, 0, "--------Cal Mode");
                    }
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
                // TODO
                
                // Update the appropriate coefficients based on cal. state
                switch (gCalState)
                {
                    case 0:     // first quarter
                        // Update the LCD
                        writeLCDString(1, 0, "Cal: 25%");
                        
                        break;
                    case 1:     // half
                        // Update the LCD
                        writeLCDString(1, 0, "Cal: 50%");

                        break;
                    case 2:     // 3rd quarter
                        // Update the LCD
                        writeLCDString(1, 0, "Cal: 75%");

                        break;
                    case 3:     // full scale
                        // Update the LCD
                        writeLCDString(1, 0, "Cal:100%");

                        break;
                }
                
                // Process calibration mode button
                if (PORTAbits.RA1)
                    nBtn2Counter++;
                else
                {
                    // Should we exit the calibration mode?
                    if (nBtn2Counter > 100)
                    {
                        // Switch the calibration mode
                        gCalState = (gCalState + 1) % 4;
                        nBtn2Counter = 0;
                        
                        // Clear the avg. buffer
                        clearAvgBuffer();
                    }
                }
 
                // Process calibration exit button
                if (PORTAbits.RA0)
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
                        writeLCDString(1, 0, "Saving..");

                        // Save coefficients in flash
                        eraseFlashStorage();
                        writeToFlash((unsigned int *)gCoefficients, 32);

                        // Update the LCD
                        writeLCDString(0, 0, "********12345678");                
                    }
                }
        }
    }
    
    return 0;
}
/**
 End of File
*/