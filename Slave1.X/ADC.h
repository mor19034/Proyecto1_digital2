/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef ADC_H
#define	ADC_H
#include <xc.h> // include processor files - each processor file is guarded.
#include <stdint.h>
#pragma config FOSC = INTRC_NOCLKOUT

void conf_ADC(uint8_t adcFrec, uint8_t isr, uint8_t Vref, uint8_t justRL);
void convert(char *data,float a, int place);
void canal_ADC(uint8_t channel);
void Select_ch(uint8_t channel);


#endif	/* XC_HEADER_TEMPLATE_H */

