/* 
 * File:   DHT11.c
* Creada por: Ing. Abiezer Hernandez O.
* Modificada por: Pablo Moreno L.
* Fecha de creacion: 10/04/2019
* Electronica y Circuitos
*
 */

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "DHT11.h"
#define _XTAL_FREQ 8000000 //utilizado para los delays

#define DHT11_PIN RA1 //puerto que se usará para comunicarse con 
                                //el sensor
#define DHT11_PIN_T TRISAbits.TRISA1 //esto servirá para cambiar entre entrada y   
                                //salida digital el pin que se utilizará

int bits[5]; //esto es una cadena donde se van a almacenar los 5 bytes que se
             //reciben del sensor

void DHT11_start(void)
{
   DHT11_PIN_T = 0; //ahora el pin actua como salida digital 
   DHT11_PIN = 0;  //señal baja de inicio de comunicación "one wire"
   __delay_ms(25); //tiempor requerido de señal baja (18ms o más)
   DHT11_PIN = 1; //señal alta para comunicación
   __delay_us(25); //tiempo requerido de señal alta (20-40us))
   DHT11_PIN_T = 1; //El pin ahora funciona como entrada digital
}

uint8_t DHT11_response(void)
{
    TMR1H = 0;                 // reset Timer1
    TMR1L = 0;
    T1CONbits.TMR1ON = 1;                // enable Timer1 module
    
    while(!DHT11_PIN && TMR1L < 100);  // wait until DHT11_PIN becomes high (checking of 80µs low time response)
    
    if(TMR1L > 99)return 0; // if response time > 99µS  ==> Response error
                            // return 0 (Device has a problem with response)
    else
    {
        TMR1H = 0;               // reset Timer1
        TMR1L = 0;
 
    while(DHT11_PIN && TMR1L < 100); // wait until DHT11_PIN becomes low (checking of 80µs high time response)
 
    if(TMR1L > 99)return 0; // if response time > 99µS  ==> Response error
                            // return 0 (Device has a problem with response)
 
    else return 1;                      // return 1 (response OK)
  } 
}

void DHT11_ReadData(uint8_t* datos){
    uint8_t i; //loop de transmision de datos
    *datos = 0;
    for(i=0; i<8; i++){
        while(!DHT11_PIN); //espera hasta que cambie a 1
        TMR1H = 0;
        TMR1L = 0;
        while(DHT11_PIN);//revisar cuanto tiempo estuvo arriba
        if(TMR1L > 50)*datos |= (1 << (7-i));
        //coloca un 1 si la señal indica que es 1, de no ser asi el valor 
        //colocado es 0 en su posicion
    }
}