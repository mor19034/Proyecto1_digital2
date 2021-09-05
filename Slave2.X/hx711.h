

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef hx711_H
#define	hx711_H

#include <xc.h> // include processor files - each processor file is guarded.  

extern sfr  sbit hx711_dat;
extern sfr sbit hx711_ck ;


extern sfr sbit hx711_dat_dir;
extern sfr sbit hx711_ck_dir ;

extern signed long offset;


void hx711_init();   //inicia el hx711
char hx711_pulso();       //un pulso para leer un bit
signed long hx711_lectura(unsigned char ganancia);   //funcion de lectura general
long hx711_promedio(unsigned char veces,unsigned char ganancia); //lectura con promedio
void tarar(unsigned char veces,unsigned char ganancia);   //captura el offset tara la balanza: numero de datos, a que ganancia

#endif	/* XC_HEADER_TEMPLATE_H */

