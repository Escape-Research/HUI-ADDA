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
const int __attribute((space(prog),aligned(_FLASH_PAGE * 2))) gFlashStorage[_FLASH_PAGE * 2] = { };

void readFromFlash(unsigned int *pRAMBuffer, int nBufferSize)
{
    int offset, i;
    
    int bufHi, bufLo;
    
    TBLPAG = __builtin_tblpage(gFlashStorage);
    offset = __builtin_tbloffset(gFlashStorage);
    offset &= 0xFBFF; // set to base of page (original: F800)
    
    for (i = 0; i < nBufferSize; i += 2)
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
    NVMADR = (offset & 0xFBFF); // for page size of 1024 PM Words (original F800)
    
    // set WREN and page erase in NVMCON
    NVMCON = 0x4003;
    
    __builtin_disi(6);      // disable interrupts
    __builtin_write_NVM();  // initiate write process
}

void writeToFlash(unsigned int *pRAMBuffer, int nBufferSize)
{
    int offset, i;
    TBLPAG = 0xFA;  // base address of write latches
    
    // Prepare the buffer
    int intBuffer[_FLASH_ROW * 2];
    int j = 0;
    for (i = 0; i < nBufferSize * 2; i++)
    {
        intBuffer[i++] = pRAMBuffer[j] & 0xFF;
        intBuffer[i] = pRAMBuffer[j++] >> 8;
    }
    
    // Load row of data into write latches
    offset = 0;
    for (i = 0; i < nBufferSize * 2; i++)
    {
        __builtin_tblwtl(offset, intBuffer[i++]);
        __builtin_tblwth(offset, intBuffer[i]);
        offset += 2;
    }

    // Set the destination address into the NVM address registers
    NVMADRU = __builtin_tblpage(gFlashStorage);
    offset = __builtin_tbloffset(gFlashStorage);
    NVMADR = (offset & 0xFBFF); // for page size of 1024 PM words (original F800)

    // Set WREN and enable row write in NVMCON
    NVMCON = 0x4002;

    __builtin_disi(6);      // disable interrupts
    __builtin_write_NVM();  // initiate write process
}


////////////////////////////////////////////////////
/*
const int __attribute__((space(prog),aligned(_FLASH_PAGE*2))) dat[_FLASH_PAGE] = {
    0xDEAD, 0xBEEF, 0x00DE, 0x00AD, 0x00BE, 0x00EF,
    0xFFFF

    // All of the other elements are initialized to zero

};

void print_uint_array(uint16_t *x, int len)
{
    int i;
    for (i = 0; i < len; i++) {
      printf(" %04X", x[i]);
    }
    printf("\n");
}

void test_flash(void)
{
    int i;
    uint16_t ram_array[_FLASH_PAGE];
    int size_ram_array = sizeof(ram_array)/sizeof(ram_array[0]);
    _prog_addressT p;
    _init_prog_address(p, dat);

    _memcpy_p2d16(ram_array, p, sizeof(ram_array));
    printf("Initial hex values:\n ");
    print_uint_array(ram_array, 10);

    _erase_flash(p);
    _memcpy_p2d16(ram_array, p, sizeof(ram_array));
    printf("After _erase_flash(p):\n ");
    print_uint_array(ram_array, 10);
 
    // And now, for something completely different in the first eight words
    for (i = 0; i < 8; i++) {
        ram_array[i] = 0x1111*(i+1);
    }
 
   // Fill the rest with all ones
    for (;i < size_ram_array; i++) {
        ram_array[i] = 0xffff;
    }
 
   // After all that, just write 10 of them as 5 pairs of 16-bit words.
    for (i = 0; i < 10; i += 2) {
        _write_flash_word32(p+2*i, ram_array[i], ram_array[i+1]);
    }
    _memcpy_p2d16(ram_array, p, sizeof(ram_array));
    printf("After loop with _write_flash_word32:\n ");
    print_uint_array(ram_array, 10);
}
*/