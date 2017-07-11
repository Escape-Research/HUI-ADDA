/*
 * File:   lcd.c
 * Author: sm186105
 *
 * Created on July 11, 2017, 2:06 PM
 */


#include "xc.h"

#include "lcd.h"

/*
 
 * 8-bit Initialization: 
// ********************************************************** 
void command(char i) 
{ 
    P1 = i;                       //put data on output Port 
    D_I =0;                       //D/I=LOW : send instruction 
    R_W =0;                       //R/W=LOW : Write 
    E  = 1;                             
    Delay(1);                     //enable pulse width >= 300ns 
    E  = 0;                       //Clock enable: falling edge 
} 

// ********************************************************** 
void write(char i) 
{ 
    P1 = i;                       //put data on output Port 
    D_I =1;                       //D/I=HIGH : send data 
    R_W =0;                       //R/W=LOW : Write 
    E  = 1; 
    Delay(1);                     //enable pulse width  >= 300ns 
    E  = 0;                       //Clock enable: falling edge 
} 
 
// ********************************************************** 
void init() 
{ 
    E = 0; 
    Delay(100);                   //Wait >40 msec after power is applied 
    command(0x30);                //command 0x30 = Wake up  
    Delay(30);                    //must wait 5ms, busy flag not available 
    command(0x30);                //command 0x30 = Wake up #2 
    Delay(10);                    //must wait 160us, busy flag not available 
    command(0x30);                //command 0x30 = Wake up #3 
    Delay(10);                    //must wait 160us, busy flag not available 
    command(0x38);                //Function set: 8-bit/2-line 
    command(0x10);                //Set cursor 
    command(0x0c);                //Display ON; Cursor ON 
    command(0x06);                //Entry mode set 
} 
// ********************************************************** 

4-bit Initialization: 
// ********************************************************** 
void command(char i) 
{ 
    P1 = i;                       //put data on output Port 
    D_I =0;                       //D/I=LOW : send instruction 
    R_W =0;                       //R/W=LOW : Write     
    Nybble();                     //Send lower 4 bits
    i = i<<4;                     //Shift over by 4 bits 
    P1 = i;                       //put data on output Port 
    Nybble();                     //Send upper 4 bits 
} 

// ********************************************************** 
void write(char i) 
{ 
    P1 = i;                       //put data on output Port 
    D_I =1;                       //D/I=HIGH : send data 
    R_W =0;                       //R/W=LOW : Write    
    Nybble();                     //Clock lower 4 bits 
    i = i<<4;                     //Shift over by 4 bits 
    P1 = i;                       //put data on output Port 
    Nybble();                     //Clock upper 4 bits 
} 
 
// ********************************************************** 
void Nybble() 
{ 
    E = 1; 
    Delay(1);                     //enable pulse width >= 300ns 
    E = 0;                        //Clock enable: falling edge 
} 
 
// ********************************************************** 
void init() 
{ 
    P1 = 0; 
    P3 = 0; 
    Delay(100);                   //Wait >40 msec after power is applied 
    P1 = 0x30;                    //put 0x30 on the output port 
    Delay(30);                    //must wait 5ms, busy flag not available 
    Nybble();                     //command 0x30 = Wake up  
    Delay(10);                    //must wait 160us, busy flag not available 
    Nybble();                     //command 0x30 = Wake up #2 
    Delay(10);                    //must wait 160us, busy flag not available 
    Nybble();                     //command 0x30 = Wake up #3 
    Delay(10);                    //can check busy flag now instead of delay 
    P1= 0x20;                     //put 0x20 on the output port 
    Nybble();                     //Function set: 4-bit interface 
    command(0x28);                //Function set: 4-bit/2-line 
    command(0x10);                //Set cursor 
    command(0x0F);                //Display ON; Blinking cursor 
    command(0x06);                //Entry Mode set 
} 
// ********************************************************** 
 
 */

