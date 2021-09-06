/* 
 * File:   Slave_2.c
 * Author: Earst
 *
 * Created on 5 de septiembre de 2021, 11:58 AM
 */

// Archivo:  Slave1_I2C.c
// Dispositivo:	PIC16F887
// Autor:    Fernando Arribas
// Compilador:	pic-as (v2.31), MPLABX V5.45
// 
// Programa: Comunicacion I2C y conversion analogica
//           
// Hardware: Potenciometros en PORTA
//           
//
// Creado: 08 aug, 2021
// Ultima modificacion: 09 aug, 2021
//*****************************************************************************
// Palabra de configuración
//*****************************************************************************
// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (RCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, RC on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

//*****************************************************************************
// Definición e importación de librerías
//*****************************************************************************
#include <stdint.h>
#include <pic16f887.h>
#include "Librerias.h"
#include <xc.h>
//*****************************************************************************
// Definición de variables
//*****************************************************************************
#define _XTAL_FREQ 8000000
uint8_t z;
uint8_t dato;
uint8_t adc;
uint8_t lec;

uint16_t lec1; 
uint8_t tem1, tem2;

uint8_t counter, speed, torque; 
//*****************************************************************************
// Definición de funciones para que se puedan colocar después del main de lo 
// contrario hay que colocarlos todas las funciones antes del main
//*****************************************************************************
void setup(void);
//*****************************************************************************
// Código de Interrupción 
//*****************************************************************************
void __interrupt() isr(void) {
    if(PIR1bits.SSPIF == 1){ 

        SSPCONbits.CKP = 0;
       
        if ((SSPCONbits.SSPOV) || (SSPCONbits.WCOL)){
            z = SSPBUF;                 // Read the previous value to clear the buffer
            SSPCONbits.SSPOV = 0;       // Clear the overflow flag
            SSPCONbits.WCOL = 0;        // Clear the collision bit
            SSPCONbits.CKP = 1;         // Enables SCL (Clock)
        }

        if(!SSPSTATbits.D_nA && !SSPSTATbits.R_nW) {
            //__delay_us(7);
            z = SSPBUF;                 // Lectura del SSBUF para limpiar el buffer y la bandera BF
            //__delay_us(2);
            PIR1bits.SSPIF = 0;         // Limpia bandera de interrupción recepción/transmisión SSP
            SSPCONbits.CKP = 1;         // Habilita entrada de pulsos de reloj SCL
            while(!SSPSTATbits.BF);     // Esperar a que la recepción se complete
            lec = SSPBUF;             // Guardar en el PORTD el valor del buffer de recepción
            __delay_us(250);
            
        }
        else if(!SSPSTATbits.D_nA && SSPSTATbits.R_nW){ //Escreitura 
            z = SSPBUF;
//            SSPBUF = tem1;
//            SSPBUF = tem2;
            switch (lec) {
                case 1: 
                    SSPBUF = tem2;
                    break;
                case 2:
                    SSPBUF = tem1;
                    break; 
            }
            SSPCONbits.CKP = 1;
            __delay_us(250);
            while(SSPSTATbits.BF);
        }
       
        PIR1bits.SSPIF = 0;    
    }
    
    //Interupcion del ADC
    if (PIR1bits.ADIF) {
        if  (ADCON0bits.CHS == 0) { //Verificamos el canal que se esta convirtiendo
            tem1 = ADRESH;
            tem2 = ADRESL;
        }
        
        else {
            PORTD = ADRESH;
        }
        
        PIR1bits.ADIF = 0;          //Reiniciamos la interupcion
    }
    
//    if (INTCONbits.T0IF ==1) 
//    {
//        counter++;  //Creamos un contador que tenga de 0 a 255 un T = 1 seg
//        INTCONbits.T0IF = 0;         // Limpiamos la bandera
//        TMR0 = 131;           //reseteamos el timer
//        
//        //Proseguimos con el bit banging de los DC
//        if (counter >= speed) { //Comparamos el valor del ADC con el contador
//            PORTDbits.RD0 = 0;  //Si el contador es mayor los puertos son 0
//        }
//        
//        else {                  //Si es menor los puertos son 1
//            PORTDbits.RD0 = 1;
//        }
//        
//        if (counter == 255) {   //Reiniciamos el contador
//            counter = 0;
//        }
//        
//    }
}
//*****************************************************************************
// Main
//*****************************************************************************
void main(void) {
    setup();
    ADCON0bits.GO   = 1;    //Damos inicio a la conversion
    
//******************************************************************************
//Loop principal
//******************************************************************************
    while(1){
        if (ADCON0bits.GO == 0){        //Cuando termine la conversion
            if (ADCON0bits.CHS == 0) {  //Verificamos cual fue el ultimo canal convertido
                ADCON0bits.CHS = 1;     //Despues cambiamos al siguiente canal
            }
            else {
                ADCON0bits.CHS = 0;
            }
            
            __delay_us(200);            //Esperamos un tiempo para que la conversion
            ADCON0bits.GO = 1;          //termine correctamente
        }
        
        lec1 = tem1 << 8;         //Dependiendo el canal guardamos el resultado
        lec1 = lec1 + tem2;
        
        
        if (lec1 > -1 && lec1 < 50) {
            speed = 255;
        }
                
        else if (lec1 > 50 && lec1 < 125) {
            speed = 180;
        }
                
        else if (lec1 > 125 && lec1 < 200) {
            speed = 120;
        }
                
        else if (lec1 > 200 && lec1 < 300) {
            speed = 80;
        }
                
        else if (lec1 > 300) {
            speed = 20;
        }
        
        torque = 255;
        PORTB = speed;
        
        CCPR1L = speed;         //Dependiendo el canal guardamos el resultado
        CCP1CONbits.DC1B1 = speed & 0b01;
        CCP1CONbits.DC1B0 = (speed >> 7);
        
        CCPR2L = torque;
        CCP2CONbits.DC2B1 = torque & 0b01;
        CCP2CONbits.DC2B0 = (torque >> 7);
        
    }
    return;
}
//*****************************************************************************
// Función de Inicialización
//*****************************************************************************
void setup(void){
    ANSEL = 0x03;
    ANSELH = 0x00;
    
    TRISA = 0x03;
    TRISB = 0x00;
    TRISD = 0x00;
    
    PORTA = 0x00;
    PORTB = 0x00;
    PORTD = 0x00;
    I2C_Slave_Init(0x50);   
    
    //Configuracion del Oscilador
    OSCCONbits.IRCF2 = 1;       //Reloj interno de 8MHz
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 1;
    OSCCONbits.SCS   = 1;
    
    //Configuracion Interupciones
    INTCONbits.GIE   = 1;       //Activamos la interupcion del ADC
    INTCONbits.PEIE  = 1;
    PIE1bits.ADIE    = 1;
    
    PIR1bits.ADIF    = 0;
    
    //Configuracion ADC
    ADCON1bits.ADFM     = 1;    //Justificado a la derecha
    ADCON1bits.VCFG0    = 0;    //Colocamos los voltajes de ref como VSS y VDD
    ADCON1bits.VCFG1    = 0;
    
    ADCON0bits.ADCS1    = 1;    //Reloj de conversion como FOSC/32
    ADCON0bits.CHS      = 0;    //Chanel 0
    __delay_us(200);
    ADCON0bits.ADON     = 1;    //Encendemos el ADC
    __delay_us(200);
    
    //Configuracion PWM
    TRISCbits.TRISC2    = 1;        //RC2/CCP1 como entrada
    TRISCbits.TRISC1    = 1;        //RC1/CCP2 como entrada
    PR2                 = 250;      //Config del periodo 2 ms
    CCP1CONbits.P1M     = 0;        //Config modo PWM
    CCP1CONbits.CCP1M   = 0b1100;   //Le indicamos el modo PWM
    CCP2CONbits.CCP2M   = 0b1100;
    
    CCPR1L              = 0x0f;     //Ciclo de trabajo inicial
    CCPR2L              = 0x0f;
    CCP1CONbits.DC1B    = 0;        //Bits memos significativos
    CCP2CONbits.DC2B0   = 0;
    CCP2CONbits.DC2B1   = 0;
    
    //Configuracion TMR2
    PIR1bits.TMR2IF     = 0;        //Limpiamos la bandera del TMR2
    T2CONbits.T2CKPS    = 0b11;     //Prescaler de 16
    T2CONbits.TMR2ON    = 1;        //Encendemos el TMR2
    
    while (PIR1bits.TMR2IF == 0);   //Esperamos a que se de una interupcion
    PIR1bits.TMR2IF     = 0;        //Limpiamos la bandera del TMR2
    
    TRISCbits.TRISC2    = 0;        //Colocamos RC1 y RC2 como salidas 
    TRISCbits.TRISC1    = 0;
}

