/*
 * File:   hx711.c
 * Author: Josue Salazar
 *
 * Created on August 31, 2021, 7:57 PM
 */


#include <xc.h>
#include <stdlib.h>


#include "hx711.h"

extern signed long offset;  //guarda el offset



void hx711_init()
{  hx711_ck_dir=0;   // salida pin reloj
   hx711_dat_dir=1;    // entrada pin dato
   hx711_ck=0; //saca cero por el reloj
   __delay_ms(500);
}

char hx711_pulso()
{
char lei;

  hx711_ck=1; //saca uno por el reloj
   __delay_us(1);
  lei=hx711_dat;
 hx711_ck=0; //saca cero por el reloj
  __delay_us(1);
  return lei;
}

signed long hx711_lectura(unsigned char ganancia)
{
signed int  hx711_cont;
signed long hx711_dato=0;


         while (hx711_dat==1);//   que no este ocuapdo el modulo
         
  //da los primeros 24 pulsos
  for (hx711_cont=23;hx711_cont>=0;hx711_cont--)
  {
     if (hx711_pulso()==1)
     {
       hx711_dato=hx711_dato +( 1L<<hx711_cont);
     }
  }     //end for
      __delay_us(100);
      
     if (ganancia==128)  hx711_pulso();
     else if (ganancia==32)
     {
      hx711_pulso();
      __delay_us(100);
      hx711_pulso();

      }
     else if (ganancia==64)
     {
      hx711_pulso();
      __delay_us(100);
      hx711_pulso();
      __delay_us(100);
      hx711_pulso();
      }


  // Verifica si es negativo
    if ((hx711_dato & 0x800000)!=0)
    {
    hx711_dato=hx711_dato | 0xff000000;  //lo convierte a 32 bit con signo
    return  (-hx711_dato);// Se coloca - si no se quiere regersar valor negado
    }
    else     return  (hx711_dato);

}

signed long hx711_promedio(unsigned char veces,unsigned char ganancia)   //numero de datos, a que ganancia
 {
        signed long prom = 0;
        unsigned char cont;

        for (cont = 0; cont < veces; cont++)
        {
          prom =prom+  hx711_lectura(ganancia);
           __delay_ms(100);
        }
        return (prom / cont);
}

void tarar(unsigned char veces,unsigned char ganancia)   //numero de datos, a que ganancia
 {
         offset=hx711_promedio(veces,ganancia);

}