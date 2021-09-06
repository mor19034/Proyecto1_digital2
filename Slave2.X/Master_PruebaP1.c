/*
 * File:   Master_PruebaP1.c
 * Author: Josue Salazar
 *
 * Created on September 5, 2021, 4:42 PM
 */


#include <xc.h>
#include "I2CP1.h"
#include "LCDP1.h"
#include "hx711.h"
#define _XTAL_FREQ 8000000
char offsett[25];
char tarart[25];
char texto[25];
uint32_t dpeso[4];
unsigned long peso;
float tpeso;
float cal; 

void floattostr(float numero_, unsigned char *cadena_,char decimales_);
void setup();
//********Conversion para LCD**********
 void floattostr_(float numero_, unsigned char *cadena_,char decimales_)
{
//variables temporales
int largo_entera,largo_n,cont_for,tempo_int;
double tempo_float;
//largo de la trama a armar en decimales
largo_n = decimales_+1;
largo_entera = 0;
// si es negativa coloca el -
if ( numero_ < 0)
  {
  *cadena_++ = '-';
  numero_ = -numero_;
  }
//si es menor que no multiplica por 10
if (numero_ > 0.0) while (numero_ < 1.0)
  {
  numero_ =numero_* 10.0;
  largo_entera--;
  }

  //realiza division varias veces hasta que sea menor que 10
while (numero_ >= 10.0)
  {
  numero_ = numero_/10.0;
  largo_entera++;   //sube el largo de la trama por ser deciaml
 }
 largo_n = largo_n+largo_entera;   //el largo es la decimal mas la parte entera

//round. numero_ is between 1 and 10 and largo_n will be printed to
// right of decimal point so rounding is ...
for (tempo_float = cont_for = 1; cont_for < largo_n; cont_for++)
  tempo_float = tempo_float/10.0;
numero_ += tempo_float/2.0;
if (numero_ >= 10.0) {numero_ = 1.0; largo_entera++;}
//si tiene decimales
if (largo_entera<0)
  {
   *cadena_++ = '0'; *cadena_++ = '.';
   if (largo_n < 0) largo_entera = largo_entera-largo_n;
   for (cont_for = -1; cont_for > largo_entera; cont_for--)  *cadena_++ = '0';
  }
for (cont_for=0; cont_for < largo_n; cont_for++)
  {
  tempo_int = numero_;
  *cadena_++ = tempo_int + 48;  //convierte a ascci
  if (cont_for ==  largo_entera ) *cadena_++ = '.';
  numero_ -= (tempo_float=tempo_int);
  numero_ = numero_*10.0;
  }
*cadena_ =0;    //anexa final de linea
}
void main(void) {
     setup();
    Lcd_Init();
    Lcd_Clear();
    while(1){
        
        I2C_Master_Start();
        I2C_Master_Write(0x60);
        I2C_Master_Write(0x03);
        I2C_Master_Stop();
        __delay_ms(200);
        
        I2C_Master_Start();
        I2C_Master_Write(0x61);
        dpeso[0]= I2C_Master_Read(0);
        I2C_Master_Stop();
        __delay_ms(200);
        
        I2C_Master_Start();
        I2C_Master_Write(0x60);
        I2C_Master_Write(0x02);
        I2C_Master_Stop();
        __delay_ms(200);
        
        I2C_Master_Start();
        I2C_Master_Write(0x61);
        dpeso[1]= I2C_Master_Read(0);
        I2C_Master_Stop();
        __delay_ms(200);
        
        I2C_Master_Start();
        I2C_Master_Write(0x60);
        I2C_Master_Write(0x01);
        I2C_Master_Stop();
        __delay_ms(200);
        
        I2C_Master_Start();
        I2C_Master_Write(0x61);
        dpeso[2]= I2C_Master_Read(0);
        I2C_Master_Stop();
        __delay_ms(200);
        
        peso = (float)dpeso[0];
        peso = (peso>>8)+ (float)dpeso[1];
        peso = (peso>>8)+ (float)dpeso[2];
        
        tpeso = peso;
        floattostr_(peso,texto,1);
        Lcd_Set_Cursor(1, 1);
        Lcd_Write_String("PESO:");
        Lcd_Set_Cursor(2, 1);
        Lcd_Write_String(texto);
   
          
    }
    return;
}
//*****************************************************************************
// Función de Inicialización
//*****************************************************************************
void setup(void){
    ANSEL = 0;
    ANSELH = 0;
    TRISA = 0;
    TRISB = 0;
    TRISD = 0;
    TRISE = 0;
    PORTA = 0;
    PORTB = 0;
    PORTD = 0;
    PORTE = 0;
     //Configuracion del Oscilador
//    config_osc(7);
    OSCCONbits.IRCF2 = 1;
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 1;
    OSCCONbits.SCS = 1;
    
    I2C_Master_Init(100000);        // Inicializar Comuncación I2C
    
}
