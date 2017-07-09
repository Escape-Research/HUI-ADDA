/*
 * File:   user.c
 * Author: sm186105
 *
 * Created on July 6, 2017, 1:14 PM
 */


#include "xc.h"
#include "user.h"

// Allocation on user flash prog 
const int __attribute((space(prog),aligned(_FLASH_PAGE * 2))) gFlashStorage[_FLASH_PAGE * 2] = { };

// The calibration coefficients in RAM
//unsigned int gCoefficients[8][4] = { 0 };
