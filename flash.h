/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

#ifndef FLASH_H
#define	FLASH_H

#include <xc.h> // include processor files - each processor file is guarded.  

// Flash storage of coefficients
extern const int __attribute((space(prog),aligned(_FLASH_PAGE * 2))) gFlashStorage[_FLASH_PAGE * 2];

void readFromFlash(unsigned int *pRAMBuffer, int nBufferSize);
void eraseFlashStorage();
void writeToFlash(unsigned int *pRAMBuffer, int nBufferSize);

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* FLASH_H */

