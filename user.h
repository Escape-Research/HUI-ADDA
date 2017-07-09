/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

#ifndef USER_H
#define	USER_H

#include <xc.h> // include processor files - each processor file is guarded.  


// Global declarations


// The calibration coefficients in RAM
extern unsigned int gCoefficients[8][4];

// Machine States
typedef enum tagRUN_STATE {
    init        = 0,
    normal      = 1,
    calibration = 2
} RUN_STATE;
extern RUN_STATE gCurrentState;



#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* USER_H */

