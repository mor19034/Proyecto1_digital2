/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef DHT11_H
#define	DHT11_H

#include <xc.h> // include processor files - each processor file is guarded. 

void DHT11_start(void);
uint8_t DHT11_response(void);
void DHT11_ReadData(uint8_t* datos);


#endif	/* XC_HEADER_TEMPLATE_H */

