/* 
 * File:   Slave_DHT11.c
 * Author: pablo
 *
 * Created on 30 de agosto de 2021, 21:43
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

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

//----------------------------librerias a utilizar------------------------------
#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // Concatenar
//#include "EUSART.h"
//#include "configuraciones_pic.h"
#include "DHT11.h"

//*******************************definiciones***********************************
#define _XTAL_FREQ 8000000 
#define DHT11_PIN PORTBbits.RB7 
//*********************************Variables************************************

 //********************************Prototipos***********************************
void setup (void);
char centenas (int dato);
char decenas (int dato);
char unidades (int dato);
 //********************************Interrupciones*******************************
// void __interrupt() isr(void){  
//--------------------------------interrupcion PORTB----------------------------


//*********************************loop principal*******************************
 void main (void){
    setup();  
    while (1){
        
    }
    return;
 }
 //*******************************funciones**************************************


 //********************************configuraciones******************************
 void setup(void){
    ANSEL = 0X00;
    ANSELH = 0X00;
    //----------------------------SELECCIONAR INPUTS Y OUTPUTS------------------   
    TRISA = 0X00;
    TRISB = 0x03;
    //----------------------------LIMPIAR PUERTOS-------------------------------    
    PORTA = 0X00;
    PORTB = 0x00;
    //------------------------------configuracion del oscilador-----------------
    OSCCONbits.IRCF = 111; ; //Reloj interno de 8MHz
    OSCCONbits.SCS = 1; //se utiliza el reloj interno del sistema
    //Configuracion del timer 1
    T1CON  = 0x10;        // Timer1 con 1:2 prescaler (Timer1 clock = 1MHz)
    TMR1H  = 0;           // reiniciar Timer1
    TMR1L  = 0; 
    //-----------------------------conf UART------------------------------------
    void init_USART (void);
 //-------------------------configuración de interrpciones----------------------

    
//    //Interrupciones de transmisión recepción activadas
//    PIE1bits.RCIE = 1;
//    PIE1bits.TXIE = 1;
//    //banderas de transmisión y recepción se bajan
//    PIR1bits.TXIF = 0; 
//    PIR1bits.RCIF = 0; 
 }

