/* 
 * File:   DHT11.c
* Creada por: Ing. Abiezer Hernandez O.
* Modificada por: Pablo Moreno L.
* Fecha de creacion: 10/04/2019
* Electronica y Circuitos
*
 */

#include <stdio.h>
#include <stdlib.h>
#include "DHT11.h"

int bits[5]; //esto es una cadena donde se van a almacenar los 5 bytes que se
             //reciben del sensor

void DHT11_start(void)
{
   DHT11_PIN_T = 0; //ahora el pin actua como salida digital 
   DHT11_PIN = 0;  //señal baja de inicio de comunicación "one wire"
   __delay_ms(25); //tiempor requerido de señal baja (18ms o más)
   DHT11_PIN = 1; //señal alta para comunicación
   __delay_us(30); //tiempo requerido de señal alta (20-40us))
   DHT11_PIN_T = 1; //El pin ahora funciona como entrada digital
}

void DHT11_response(void)
{
    TMR1H = 0;                 // reset Timer1
    TMR1L = 0;
    TMR1ON = 1;                // enable Timer1 module
    
    while(!DHT11_PIN && TMR1L < 100);  // wait until DHT11_PIN becomes high (checking of 80µs low time response)
    
    if(TMR1L > 99)                     // if response time > 99µS  ==> Response error
    return 0;                        // return 0 (Device has a problem with response)
 
    else
    {
        TMR1H = 0;               // reset Timer1
        TMR1L = 0;
 
    while(DHT11_PIN && TMR1L < 100); // wait until DHT11_PIN becomes low (checking of 80µs high time response)
 
    if(TMR1L > 99)                   // if response time > 99µS  ==> Response error
      return 0;                      // return 0 (Device has a problem with response)
 
    else
      return 1;                      // return 1 (response OK)
  } 
}

int DHT11_read_byte(void)
{
   int i,data = 0;  
   for(i=0;i<8;i++){  
        TMR1H = 0;             // reset Timer1
        TMR1L = 0;
      while(!DHT11_PIN){
      }
      __delay_us(50);         
      if(DHT11_PIN){  
         data = ((data<<1) | 1); 
      }
      else{
         data = (data<<1);  
      }
      while(DHT11_PIN){
      }
   }
   return data;
}

int DHT11_read_data(float *hum, float *temp)
{
   char buf[8];
   int result = 0;
   DHT11_start();
   DHT11_response();
   bits[0] = DHT11_read_byte();   //Humedad entero
   bits[1] = DHT11_read_byte();   //Humedad decimal
   bits[2] = DHT11_read_byte();   //Temp entero
   bits[3] = DHT11_read_byte();   //Temp decimal
   bits[4] = DHT11_read_byte();   //Paridad
   result = bits[0] + bits[1] + bits[2] + bits[3];
   if (result == bits[4]){        //si la los datos son correctos 
      sprintf(buf, "%2d.%1d",bits[0],bits[1]);
      *hum = atof(buf);
      sprintf(buf, "%2d.%1d",bits[2],bits[3]);
      *temp = atof(buf);  
      return 1;
   }
   else{
      return 0;
   }
}
