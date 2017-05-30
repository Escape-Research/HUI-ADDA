/*******************************************************************************
  DMA Generated Driver File

Company:
Microchip Technology Inc.

File Name:
    dma.c

Summary:
This is the generated driver implementation file for the DMA driver using MPLAB(c) Code Configurator

Description:
This source file provides implementations for driver APIs for DMA.
Generation Information :
Product Revision  :  MPLAB(c) Code Configurator - pic24-dspic-pic32mm : v1.26
Device            :  dsPIC33EV256GM102
The generated drivers are tested against the following:
Compiler          :  XC16 1.30
MPLAB             :  MPLAB X 3.45
*******************************************************************************/

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

#include <xc.h>
#include "dma.h"
void DMA_Initialize(void) 
{ 
    // Initialize channels which are enabled 
    // AMODE Register Indirect with Post-Increment mode; CHEN enabled; DIR Reads from peripheral address, writes to RAM address; HALF Initiates interrupt when all of the data has been moved; SIZE 16 bit; NULLW disabled; MODE Continuous, Ping-Pong modes are disabled; 
    DMA0CON= 0x8000 & 0xFFFE; //Enable DMA Channel later;
    // FORCE disabled; IRQSEL SPI1; 
    DMA0REQ= 0xA;
    // CNT 0; 
    DMA0CNT= 0x0;
    // STA 4096; 
    DMA0STAL= 0x1000;
    // STA 0; 
    DMA0STAH= 0x0;
    // Clearing Channel 0 Interrupt Flag;
    IFS0bits.DMA0IF = false;
    // Enabling Channel 0 Interrupt
    IEC0bits.DMA0IE = 1;
    // AMODE Register Indirect with Post-Increment mode; CHEN enabled; SIZE 16 bit; DIR Reads from RAM address, writes to peripheral address; NULLW disabled; HALF Initiates interrupt when all of the data has been moved; MODE Continuous, Ping-Pong modes are disabled; 
    DMA1CON= 0xA000 & 0xFFFE; //Enable DMA Channel later;
    // FORCE disabled; IRQSEL SPI2; 
    DMA1REQ= 0x21;
    // CNT 0; 
    DMA1CNT= 0x0;
    // STA 4096; 
    DMA1STAL= 0x1000;
    // STA 0; 
    DMA1STAH= 0x0;
    // Clearing Channel 1 Interrupt Flag;
    IFS0bits.DMA1IF = false;
    // Enabling Channel 1 Interrupt

    //Enable DMA Channel 0
    
    DMA0CONbits.CHEN = 1;
    //Enable DMA Channel 1
    
    DMA1CONbits.CHEN = 1;
}
void __attribute__ ( ( interrupt, no_auto_psv ) ) _DMA0Interrupt( void )
{
    IFS0bits.DMA0IF = 0;
}
void __attribute__ ( ( interrupt, no_auto_psv ) ) _DMA1Interrupt( void )
{
    IFS0bits.DMA1IF = 0;
}

/**
  End of File
*/
