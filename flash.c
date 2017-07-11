/*
 * File:   flash.c
 * Author: stefanos
 *
 * Created on July 8, 2017, 8:14 AM
 */


#include "xc.h"
#include "user.h"
#include "flash.h"

// Allocation on user flash prog 
const unsigned int __attribute((space(prog),aligned(_FLASH_PAGE * 2))) gFlashStorage[_FLASH_PAGE * 2] = { 
    _16BIT_1Q, _16BIT_HALF, _16BIT_3Q, _16BIT_FS,
    _16BIT_1Q, _16BIT_HALF, _16BIT_3Q, _16BIT_FS,
    _16BIT_1Q, _16BIT_HALF, _16BIT_3Q, _16BIT_FS,
    _16BIT_1Q, _16BIT_HALF, _16BIT_3Q, _16BIT_FS,
    _16BIT_1Q, _16BIT_HALF, _16BIT_3Q, _16BIT_FS,
    _16BIT_1Q, _16BIT_HALF, _16BIT_3Q, _16BIT_FS,
    _16BIT_1Q, _16BIT_HALF, _16BIT_3Q, _16BIT_FS,
    _16BIT_1Q, _16BIT_HALF, _16BIT_3Q, _16BIT_FS
};

void readFromFlash(unsigned int *pRAMBuffer, int nBufferSize)
{
    int offset, i;
    
    int bufHi, bufLo;
    
    TBLPAG = __builtin_tblpage(gFlashStorage);
    offset = __builtin_tbloffset(gFlashStorage);
    offset &= 0xFBFF; // set to base of page (original: F800)
    
    for (i = 0; i < nBufferSize * 2; i += 2)
    {
        // Load the 32 bit value from flash into the intermediate buffer
        bufLo = __builtin_tblrdl(offset);
        bufHi = __builtin_tblrdh(offset);
        
        // Convert to a single 24bit value
        pRAMBuffer[i >> 1] = (bufHi << 8) + bufLo;
        
        offset += 2;
    }
}

void eraseFlashStorage()
{
    int offset;
    
    NVMADRU = __builtin_tblpage(gFlashStorage);
    offset = __builtin_tbloffset(gFlashStorage);
    NVMADR = (offset & 0xFBFF); // for page size of 512 PM Words (original F800)
    
    // set WREN and page erase in NVMCON
    NVMCON = 0x4003;
    
    __builtin_disi(6);      // disable interrupts
    __builtin_write_NVM();  // initiate write process
    
    // wait until the operation is complete
    while (NVMCONbits.WR == 1)
        ;
}

void writeToFlash(unsigned int *pRAMBuffer, int nBufferSize)
{
    int offset, i;
    
    // Initialize and prepare the buffer
    unsigned int intBuffer[_FLASH_ROW * 2] = { };
    int j = 0;
    for (i = 0; i < nBufferSize * 2; i++)
    {
        intBuffer[i++] = pRAMBuffer[j++];
        intBuffer[i] = 0x00;
    }
    // Setup the source location for the copy
    NVMSRCADRL = (int)&intBuffer[0]; // Start address of data in RAM
        
    // Set the destination address into the NVM address registers
    NVMADRU = __builtin_tblpage(gFlashStorage);
    offset = __builtin_tbloffset(gFlashStorage);
    NVMADR = (offset & 0xFBFF); // for page size of 512 PM words (original F800)
    
    // Set WREN and enable row write in NVMCON
    NVMCON = 0x4002;
    NVMCONbits.RPDF = 0; // Select uncompressed format

    __builtin_disi(6);      // disable interrupts
    __builtin_write_NVM();  // initiate write process
    
    // wait until the operation is complete
    while (NVMCONbits.WR == 1)
        ;
}
