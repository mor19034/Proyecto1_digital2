/*
 * File:   Slave2.c
 * Author: Josue Salazar
 *
 * Created on August 31, 2021, 7:50 PM
 */



#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF         // Low Voltage Programming Enable bit (RB3/PGM pin has PGM function, low voltage programming enabled)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // Concatenar

#include "hx711.h" 
#include "LCDP1.h"
#include "I2CP1.h"
#include "DHT11.h"
#define _XTAL_FREQ 8000000

//*********Variables***********
//int hx711_dat;
//int hx711_ck;
//
//
//int hx711_dat_dir;
//int hx711_ck_dir;

//*********************************definiciones*********************************
#define hx711_dat  PORTDbits.RD6   //pin de datos del micro
#define hx711_ck   PORTDbits.RD7     // pin de reloj


#define hx711_dat_dir  TRISDbits.TRISD6
#define hx711_ck_dir   TRISDbits.TRISD7

#define DHT11_PIN PORTAbits.RA1 
#define DHT11_PIN_Direction TRISAbits.TRISA1
//*******************************variables**************************************
const float offset_manual = 83877;
const float calibra = 419386.9 - offset_manual;
const float prueba = 1000.0;

float factor = prueba/calibra;
signed long offset = offset_manual;

signed long celda =0;
char texto[25];
char offsett[25];
char tarart[25];
float peso;
float cal; 
uint8_t contador;
uint8_t Temp1;
uint8_t dummyT1;
uint8_t Hum1;
uint8_t dummyHum1;
uint8_t CHECKSUM;
uint8_t z;
uint8_t lectura;
//**********Prototipos*************
void setup(void);
void floattostr(float numero_, unsigned char *cadena_,char decimales_);
void ISR (void);
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
void __interrupt() ISR(void){
    if (PIR1bits.ADIF){
            if(ADCON0bits.CHS == 0){
            CCPR1L = (ADRESH>>1)+125;
            PORTC++;
        }
        else{
            CCPR2L = (ADRESH>>1)+125;
        }
        PIR1bits.ADIF = 0;
    }
     //--------------------------------interrupion I2C------------------------------    
   if(PIR1bits.SSPIF == 1){ 
        
        SSPCONbits.CKP = 0;
       
        if ((SSPCONbits.SSPOV) || (SSPCONbits.WCOL)){
            z = SSPBUF;                 // Read the previous value to clear the buffer
            SSPCONbits.SSPOV = 0;       // Clear the overflow flag
            SSPCONbits.WCOL = 0;        // Clear the collision bit
            SSPCONbits.CKP = 1;         // Enables SCL (Clock)
            
        }
//----------------------------hacer al esclavo leer-----------------------------
        if(!SSPSTATbits.D_nA && !SSPSTATbits.R_nW) {
            //__delay_us(7);
            z = SSPBUF;                 // Lectura del SSBUF para limpiar el buffer y la bandera BF
            //__delay_us(2);
            PIR1bits.SSPIF = 0;         // Limpia bandera de interrupción recepción/transmisión SSP
            SSPCONbits.CKP = 1;         // Habilita entrada de pulsos de reloj SCL
            while(!SSPSTATbits.BF);     // Esperar a que la recepción se complete
            lectura = SSPBUF;             // Guardar en el PORTD el valor del buffer de recepción
            __delay_us(250);
//------------------------------rl esclavo manda datos--------------------------         
        }else if(!SSPSTATbits.D_nA && SSPSTATbits.R_nW){
            z = SSPBUF;
            BF = 0;
            SSPBUF = Hum1; //manda el la conversion del ADC
            SSPCONbits.CKP = 1;
            __delay_us(250);
            while(SSPSTATbits.BF);
        }
       
        PIR1bits.SSPIF = 0;    
    }
    return;
}

 
void main(void) {
    
    setup();
    hx711_init();
    floattostr_(offset,offsett,1);
    Lcd_Init();
    Lcd_Clear();
     Lcd_Set_Cursor(1, 8);
     Lcd_Write_String("Off:");
     Lcd_Set_Cursor(2, 8);
     Lcd_Write_String(offsett);
     ADCON0bits.GO = 1;     
    while(1){
        DHT11_start();
        if(DHT11_response()){
            DHT11_ReadData(&Hum1);
            DHT11_ReadData(&dummyHum1);
            DHT11_ReadData(&Temp1);
            DHT11_ReadData(&dummyT1);
            DHT11_ReadData(&CHECKSUM);
        }
        __delay_ms(500);
        if(ADCON0bits.GO == 0){
            if (ADCON0bits.CHS == 0){
                ADCON0bits.CHS = 1;
            }
            else {
                ADCON0bits.CHS = 0;
                
            }
            __delay_us(200);
            ADCON0bits.GO = 1;
        }
    // peso=hx711_lectura(128)-(float)offset;
    peso=hx711_promedio(10,128)-(float)offset;
    peso=peso*factor;
   if (peso<=0) peso=0;
     floattostr_(peso,texto,1);
     Lcd_Set_Cursor(1, 1);
     Lcd_Write_String("PESO:");
     Lcd_Set_Cursor(2, 1);
     Lcd_Write_String(texto);
   
    if(PORTDbits.RD5==1)
   {
    tarar(10,128);  //numero de datos, a que ganancia
    floattostr_(offset,tarart,1);
    Lcd_Set_Cursor(2, 8);
    Lcd_Write_String(tarart);
    
   }
   
     if (PORTDbits.RD4==1)
   {
   cal=hx711_promedio(50,128)-(float)offset;
   factor= prueba/cal;
   
   }
    
    }
    return;
}
//*****************************************************************************
// Función de Inicialización
//*****************************************************************************
void setup(void){
    ANSEL = 0b00000011;
    ANSELH = 0;
    TRISA = 0X03;
    TRISDbits.TRISD4 = 1;
    TRISDbits.TRISD5 = 1;
    TRISDbits.TRISD6 = 1;
    TRISDbits.TRISD7 = 1;
    TRISB = 0;
    TRISE = 0;
 
    PORTA = 0;
    PORTB = 0;
    PORTD = 0;
    PORTE = 0;
    I2C_Slave_Init(0X10); //inicializar comunicación I2C del esclavo y se le da
    //dirección al esclavo, en este caso 0x50
    
     //Configuracion del Oscilador
//    config_osc(7);
    OSCCONbits.IRCF2 = 1;
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 1;
    OSCCONbits.SCS = 1;
    //Configuracion del ADC
  ADCON1bits.ADFM = 0;
  ADCON1bits.VCFG0 = 0;
  ADCON1bits.VCFG1 = 0;
  
  ADCON0bits.ADCS = 0b10;
  ADCON0bits.CHS = 0;
  __delay_us(200);
  ADCON0bits.ADON = 1;
  __delay_us(200);
     // cinfuguracion de PWM
  TRISCbits.TRISC2 = 1;
  TRISCbits.TRISC1 = 1;
  PR2 = 250;
  CCP1CONbits.P1M = 0;
  CCP2CONbits.CCP2M = 0b1100;
  CCP1CONbits.CCP1M = 0b1100;
  
  CCPR1L = 0X0F;
  CCPR2L = 0X0F;
  CCP1CONbits.DC1B = 0;
  CCP2CONbits.DC2B0 = 0;
  CCP2CONbits.DC2B1 = 0;
  
  //Configuracion del TMR2
  PIR1bits.TMR2IF = 0;
  T2CONbits.T2CKPS = 0b11;
  T2CONbits.TMR2ON = 1;
  //Configuracion del timmer1
  T1CON = 0X10; //periodo de 1MHz, ya que fuente es FOSC/4
  TMR1H = 0; //valores en 0 para el conteo de los periodos de pulso
  TMR1L = 0;  
  while(PIR1bits.TMR2IF == 0);
  PIR1bits.TMR2IF = 0;
  TRISCbits.TRISC2 = 0;
  TRISCbits.TRISC1 = 0;
  
  //Configuracion de interrupciones
  PIR1bits.ADIF = 0;
  PIE1bits.ADIE = 1;
  INTCONbits.PEIE = 1;
  INTCONbits.GIE = 1;
  return;
    
}
