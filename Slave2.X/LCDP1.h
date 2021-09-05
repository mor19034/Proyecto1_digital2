/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef LCDP1_H
#define	LCDP1_H
  
#pragma config FOSC = INTRC_NOCLKOUT
#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>

#define RS PORTEbits.RE0
#define EN PORTEbits.RE2
#define RW PORTEbits.RE1    
#define D0 PORTBbits.RB0
#define D1 PORTBbits.RB1
#define D2 PORTBbits.RB2
#define D3 PORTBbits.RB3
#define D4 PORTBbits.RB4
#define D5 PORTBbits.RB5
#define D6 PORTBbits.RB6
#define D7 PORTBbits.RB7
        
void Lcd_Port(char a);

void Lcd_Cmd(char a);

void Lcd_Clear(void);

void Lcd_Set_Cursor(char a, char b);

void Lcd_Init(void);

void Lcd_Write_Char(char a);

void Lcd_Write_String(char *a);

void Lcd_Shift_Right(void);

void Lcd_Shift_Left(void);

#endif	/* XC_HEADER_TEMPLATE_H */

