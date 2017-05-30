/*******************************************************************************
  DMA Generated Driver API Header File

  Company:
    Microchip Technology Inc.

  File Name:
    dma.h

  Summary:
    This is the generated header file for the DMA driver using MPLAB(c) Code Configurator

  Description:
    This header file provides APIs for driver for DMA.
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

#ifndef DMA_H
#define DMA_H

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
        
/**
  Section: Data Types
*/

/** DMA Channel Definition
 
 @Summary 
   Defines the channels available for DMA
 
 @Description
   This routine defines the channels that are available for the module to use.
 
 Remarks:
   None
 */
typedef enum 
{
    DMA_CHANNEL_0 =  0,       
    DMA_CHANNEL_1 =  1,       
    DMA_CHANNEL_2 =  2,       
    DMA_CHANNEL_3 =  3,       
    DMA_NUMBER_OF_CHANNELS = 4
} DMA_CHANNEL;
/**
  Section: Interface Routines
*/

/**
  @Summary
    This function initializes DMA instance : 

  @Description
    This routine initializes the DMA driver instance for : 
    index, making it ready for clients to open and use it. It also initializes any
    internal data structures.
    This routine must be called before any other DMA routine is called. 

  @Preconditions
    None.

  @Param
    None.

  @Returns
    None.

  @Comment
   
 
  @Example
    <code>
        unsigned short int srcArray[100];
        unsigned short int dstArray[100];
        int i;
        int count;
        for (i=0; i<100; i++)
        {
            srcArray[i] = i+1;
            dstArray[i] = 0;
        }
        
        DMA_Initialize();
        DMA_SoftwareTriggerEnable(CHANNEL1);
        
        count = DMA_TransferCountGet;
        while(count > 0)
        {
        while(
        DMA_SoftwareTriggerEnable(CHANNEL1);
        }
    </code>

*/
void DMA_Initialize(void);

/**
  @Summary
    Clears the interrupt request flag.

  @Description
    This routine is used to clear the interrupt request flag. This routine sets the value of the DMAIF bit to 0.
 
  @Preconditions
    DMA_Initializer() function should have been 
    called before calling this function.
 
  @Returns
    None

  @Param
    None
  
  @Example
    Refer to DMA_Initializer(); for an example
 */
inline static void DMA_FlagInterruptClear(DMA_CHANNEL  channel)
{    
    switch(channel) { 
        case DMA_CHANNEL_0:
                IFS0bits.DMA0IF = 0;                
                break;       
        case DMA_CHANNEL_1:
                IFS0bits.DMA1IF = 0;                
                break;       
        case DMA_CHANNEL_2:
                IFS1bits.DMA2IF = 0;                
                break;       
        case DMA_CHANNEL_3:
                IFS2bits.DMA3IF = 0;                
                break;       
        default:break;
         
    }
}

/**
@Summary
    Enables the interrupt for a DMA channel

  @Description
    This routine is used to enable an interrupt for a DMA channel. This routine
    sets the value of the DMAIE bit to 1.
 
  @Preconditions
    DMA_Initializer() function should have been 
    called before calling this function.
 
  @Returns
    None

  @Param
    None
  
  @Example
    Refer to DMA_Initializer(); for an example
 */ 
inline static void DMA_InterruptEnable(DMA_CHANNEL  channel)
{    
    switch(channel) { 
        case DMA_CHANNEL_0:
                IEC0bits.DMA0IE = 1;                
                break;       
        case DMA_CHANNEL_1:
                IEC0bits.DMA1IE = 1;                
                break;       
        case DMA_CHANNEL_2:
                IEC1bits.DMA2IE = 1;                
                break;       
        case DMA_CHANNEL_3:
                IEC2bits.DMA3IE = 1;                
                break;       
        default:break;
         
    }
}
/**
@Summary
 Disables the interrupt for a DMA channel

@Description
 This routine is used to disable an interrupt for a DMA channel. This routine
 sets the value of the DMAIE bit to 0.
 
@Preconditions
  DMA_Initializer() function should have been 
  called before calling this function.
 
@Returns
 None

@Param
 None
  
@Example
 Refer to DMA_Initializer(); for an example
*/  
inline static void DMA_InterruptDisable(DMA_CHANNEL  channel)
{    
    switch(channel) {        
        case DMA_CHANNEL_0:
                IEC0bits.DMA0IE = 0;                
                break;       
        case DMA_CHANNEL_1:
                IEC0bits.DMA1IE = 0;                
                break;       
        case DMA_CHANNEL_2:
                IEC1bits.DMA2IE = 0;                
                break;       
        case DMA_CHANNEL_3:
                IEC2bits.DMA3IE = 0;                
                break;       
        default:break;
    }
}
    
/**
 @Summary
  Enables the channel in the DMA

 @Description
  This routine is used to enable a channel in the DMA. This routine
  sets the value of the CHEN bit to 1.
 
 @Preconditions
  DMA_Initializer() function should have been 
  called before calling this function.
 
 @Returns
  None

 @Param
  None
  
 @Example
 Refer to DMA_Initializer(); for an example
 */
inline static void DMA_ChannelEnable(DMA_CHANNEL  channel)
{
    switch(channel) {
        case DMA_CHANNEL_0:
                DMA0CONbits.CHEN = 1;
                break;     
        case DMA_CHANNEL_1:
                DMA1CONbits.CHEN = 1;
                break;     
        case DMA_CHANNEL_2:
                DMA2CONbits.CHEN = 1;
                break;     
        case DMA_CHANNEL_3:
                DMA3CONbits.CHEN = 1;
                break;     
        default: break;
    }
}
    
/**
 @Summary
 Disables the channel in the DMA

 @Description
  This routine is used to disable a channel in the DMA. This routine
  sets the value of the CHEN bit to 0.
 
@Preconditions
 DMA_Initializer() function should have been 
 called before calling this function.
 
@Returns
 None

@Param
 None
  
@Example
 Refer to DMA_Initializer(); for an example
*/
inline static void DMA_ChannelDisable(DMA_CHANNEL  channel)
{
    switch(channel) {
        case DMA_CHANNEL_0:
                DMA0CONbits.CHEN = 0;
                break;    
        case DMA_CHANNEL_1:
                DMA1CONbits.CHEN = 0;
                break;    
        case DMA_CHANNEL_2:
                DMA2CONbits.CHEN = 0;
                break;    
        case DMA_CHANNEL_3:
                DMA3CONbits.CHEN = 0;
                break;    
        default: break;
    }
}
/**
  @Summary
    Sets the transfer count of the DMA

  @Description
    This routine is used to set the DMA transfer count. This routine sets the
    value of the DMACNT register. 
 
  @Preconditions
    DMA_Initializer() function should have been 
    called before calling this function.
 
  @Returns
    None

  @Param
    None
  
  @Example
    Refer to DMA_Initializer(); for an example
 */
inline static void DMA_TransferCountSet(DMA_CHANNEL channel, uint16_t transferCount)
{
    switch(channel) {
        case DMA_CHANNEL_0:
                DMA0CNT = transferCount;
                break;
        case DMA_CHANNEL_1:
                DMA1CNT = transferCount;
                break;
        case DMA_CHANNEL_2:
                DMA2CNT = transferCount;
                break;
        case DMA_CHANNEL_3:
                DMA3CNT = transferCount;
                break;
        default: break;
    }
}
/**
 @Summary
  Returns the transfer count of the DMA

@Description
 This routine is used to determine the DMA transfer count. This routine
 returns the value of the DMACNT register. 
 
@Preconditions
  DMA_Initializer() function should have been 
  called before calling this function.
 
@Returns
 Returns the transfer count of the DMA

@Param
 None
  
@Example
 Refer to DMA_Initializer(); for an example
*/
inline static uint16_t DMA_TransferCountGet(DMA_CHANNEL channel)
    {
    switch(channel) {
            case DMA_CHANNEL_0:
                return (DMA0CNT);               
            case DMA_CHANNEL_1:
                return (DMA1CNT);               
            case DMA_CHANNEL_2:
                return (DMA2CNT);               
            case DMA_CHANNEL_3:
                return (DMA3CNT);               
        default: return 0;
    }
}

/**
 @Summary
  Initiates a transfer on the requested DMA channel.

 @Description
  This routine is used to initiate a transfer on the requested DMA channel. When a transfer on the requested channel is initiated the routine
  returns the value of the FORCE bit. It returns 0 otherwise.
 
 @Preconditions
   DMA_Initializer() function should have been 
   called before calling this function.
 
 @Returns
  Returns true if the transfer on the requested channel is initiated 

 @Param
  None
  
 @Example
  Refer to DMA_Initializer(); for an example
 */
inline static void DMA_SoftwareTriggerEnable(DMA_CHANNEL channel )
{
    switch(channel) {
        case DMA_CHANNEL_0:
                DMA0REQbits.FORCE = 1;   
        case DMA_CHANNEL_1:
                DMA1REQbits.FORCE = 1;   
        case DMA_CHANNEL_2:
                DMA2REQbits.FORCE = 1;   
        case DMA_CHANNEL_3:
                DMA3REQbits.FORCE = 1;   
        default: break;
    }
}  
    
/**
  @Summary
    Sets the address for register A in the DMA

  @Description
    This routine is used to set the address in register A for a DMA channel. 
 
  @Preconditions
    DMA_Initializer() function should have been 
    called before calling this function.
 
  @Returns
    None

  @Param
    None
  
  @Example
    Refer to DMA_Initializer(); for an example
 */
inline static void DMA_StartAddressASet(DMA_CHANNEL  channel, uint16_t address) 
{
    switch(channel) {
        case DMA_CHANNEL_0:
                DMA0STAL = address;
                DMA0STAH = 0;
                break;        
        case DMA_CHANNEL_1:
                DMA1STAL = address;
                DMA1STAH = 0;
                break;        
        case DMA_CHANNEL_2:
                DMA2STAL = address;
                DMA2STAH = 0;
                break;        
        case DMA_CHANNEL_3:
                DMA3STAL = address;
                DMA3STAH = 0;
                break;        
        default: break;
    }    
    }
    
/**
  @Summary
    Sets the address for register B in the DMA

  @Description
    This routine is used to set the address in register B for a DMA channel. 
 
  @Preconditions
    DMA_Initializer() function should have been 
    called before calling this function.
 
  @Returns
    None

  @Param
    None
  
  @Example
    Refer to DMA_Initializer(); for an example
 */
 
inline static void DMA_StartAddressBSet(DMA_CHANNEL  channel, uint16_t address) 
{
    switch(channel) {
        case DMA_CHANNEL_0:
                DMA0STBL = address;
                DMA0STBH = 0;
                break;       
        case DMA_CHANNEL_1:
                DMA1STBL = address;
                DMA1STBH = 0;
                break;       
        case DMA_CHANNEL_2:
                DMA2STBL = address;
                DMA2STBH = 0;
                break;       
        case DMA_CHANNEL_3:
                DMA3STBL = address;
                DMA3STBH = 0;
                break;       
        default: break;
    }    
}
    /**
  @Summary
    Gets the address for register A in the DMA

  @Description
    This routine is used to get the address in register A for a DMA channel. 
 
  @Preconditions
    DMA_Initializer() function should have been 
    called before calling this function.
 
  @Returns
    None

  @Param
    None
  
  @Example
    Refer to DMA_Initializer(); for an example
 */
inline static uint16_t DMA_StartAddressAGet(DMA_CHANNEL  channel, uint16_t address) 
{    
    switch(channel) {
        case DMA_CHANNEL_0:
                address= DMA0STAL;       
                break;       
        case DMA_CHANNEL_1:
                address= DMA1STAL;       
                break;       
        case DMA_CHANNEL_2:
                address= DMA2STAL;       
                break;       
        case DMA_CHANNEL_3:
                address= DMA3STAL;       
                break;       
        default:
                address = 0;
                break;
    }
        return address;
            
}
    /**
  @Summary
    Sets the address for register B in the DMA

  @Description
    This routine is used to set the address in register B for a DMA channel. 
 
  @Preconditions
    DMA_Initializer() function should have been 
    called before calling this function.
 
  @Returns
    None

  @Param
    None
  
  @Example
    Refer to DMA_Initializer(); for an example
 */
 
inline static uint16_t DMA_StartAddressBGet(DMA_CHANNEL  channel, uint16_t address) {
        
    switch(channel) {
        case DMA_CHANNEL_0:
                address= DMA0STBL;       
                break;        
        case DMA_CHANNEL_1:
                address= DMA1STBL;       
                break;        
        case DMA_CHANNEL_2:
                address= DMA2STBL;       
                break;        
        case DMA_CHANNEL_3:
                address= DMA3STBL;       
                break;        
        default:
                address = 0;
                break;
    }
        return address;
}
    /**
    @Summary
    Sets the peripheral address in the DMA

  @Description
    This routine is used to set the peripheral address for a DMA channel. 
 
  @Preconditions
    DMA_Initializer() function should have been 
    called before calling this function.
 
  @Returns
    None

  @Param
    None
  
  @Example
    Refer to DMA_Initializer(); for an example
 */
 inline static void DMA_PeripheralAddressSet(DMA_CHANNEL channel, volatile unsigned int * address)
    {
    switch(channel) {
        case DMA_CHANNEL_0:
                DMA0PAD = (int)address;
                break;   
        case DMA_CHANNEL_1:
                DMA1PAD = (int)address;
                break;   
        case DMA_CHANNEL_2:
                DMA2PAD = (int)address;
                break;   
        case DMA_CHANNEL_3:
                DMA3PAD = (int)address;
                break;   
        default: break;
    }
}
/**
  @Summary
    Returns true when there is a Peripheral Write Collision Event

  @Description
    This routine is used to determine if there is a Peripheral Write Collision Event. This routine
    returns the value of the PWCOL bit in DMAPWC register. When there is a Peripheral Write Collision Event, the routine
    returns 1. It returns 0 otherwise.
 
  @Preconditions
    DMA_Initializer() function should have been 
    called before calling this function.
 
  @Returns
    Returns true if there is a Peripheral Write Collision Event

  @Param
    None
  
  @Example
    Refer to DMA_Initializer(); for an example
 */
inline static bool DMA_IsPeripheralWriteCollision(uint16_t dmaChannel )
{
    return DMAPWC & (1 << dmaChannel);
}

/**
  @Summary
    Returns true when there is a Request Collision Event

  @Description
    This routine is used to determine if there is a Request Collision Event. This routine
    returns the value of the RQCOL bit in DMARQC register. When there is a Request Collision Event, the routine
    returns 1. It returns 0 otherwise.
 
  @Preconditions
    DMA_Initializer() function should have been 
    called before calling this function.
 
  @Returns
    Returns true if there is a Request Collision Event

  @Param
    None
  
  @Example
    Refer to DMA_Initializer(); for an example
 */
inline static bool DMA_IsRequestCollision(uint16_t dmaChannel )
{
    return DMARQC & (1 << dmaChannel);
}

/**
  @Summary
    Sets the requested DMA Channel IRQ Select register with the requested peripheral IRQ number.

  @Description
    This routine is used to set the requested DMA Channel IRQ Select register with the requested peripheral IRQ number.
 
  @Preconditions
    DMA_Initializer() function should have been 
    called before calling this function.
 
  @Returns
    None

  @Param
    None
  
  @Example
    Refer to DMA_Initializer(); for an example
 */
inline static void DMA_PeripheralIrqNumberSet(DMA_CHANNEL channel, uint8_t irqNumber)
{
    switch(channel) {
        case DMA_CHANNEL_0:
                DMA0REQ = irqNumber;
                break;   
        case DMA_CHANNEL_1:
                DMA1REQ = irqNumber;
                break;   
        case DMA_CHANNEL_2:
                DMA2REQ = irqNumber;
                break;   
        case DMA_CHANNEL_3:
                DMA3REQ = irqNumber;
                break;   
        default: break;
    }
}

#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
    
#endif  // DMA.h


/**
  End of File
*/
