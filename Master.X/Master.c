// Archivo:  Master.c
// Dispositivo:	PIC16F887
// Autor:    Fernando Arribas, Pablo Moreno y Josue Salazar
// Compilador:	pic-as (v2.31), MPLABX V5.45
// 
// Programa: Comunicacion I2C, Eusart y pantalla LCD
//           
// Hardware: LCD en PORTB
//           
//
// Creado: 05 sep, 2021
// Ultima modificacion: 05 sep, 2021
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
#include <pic16f887.h>
#include "Librerias.h"
#include <xc.h>
#include <stdint.h>
#include <stdio.h>  // Para usar printf
#include <string.h> // Concatenar
#include <stdlib.h> //Recibir numeros
//*****************************************************************************
// Definición de variables
//*****************************************************************************
#define _XTAL_FREQ 8000000

//******************************************************************************
// Variables
//****************************************************************************
volatile uint8_t tem1 = 0;
volatile uint8_t tem2 = 0;
volatile uint8_t contador = 0;
float temp;
float volt, temperatura;

char lcd1[10];
char lcd2[10];
char lcd3[10];

float conv1 = 0;
float conv2 = 0;
float conv3 = 0;

uint8_t datos_sensor[3];
int dato;
//*****************************************************************************
// Definición de funciones para que se puedan colocar después del main de lo 
// contrario hay que colocarlos todas las funciones antes del main
//*****************************************************************************
void setup(void);


//*****************************************************************************
// Main
//*****************************************************************************
void main(void) {
    setup();
    Lcd_Init();
    Lcd_Clear();
    while(1){
        //Primera linea del LCD
        Lcd_Set_Cursor(1, 1);
        Lcd_Write_String("ADC");
        Lcd_Set_Cursor(1, 8);
        Lcd_Write_String("SEN");
        Lcd_Set_Cursor(1, 14);
        Lcd_Write_String("CON");
        
        //Lectura Esclavo 1
        I2C_Master_Start();
        I2C_Master_Write(0x50);   //Address del sensor TC74
        I2C_Master_Write(0x02); //Comando del sensor
        I2C_Master_Stop();
        __delay_ms(200);
        
        I2C_Master_Start();
        I2C_Master_Write(0x51);
        tem1 = I2C_Master_Read(0);
        I2C_Master_Stop();
        __delay_ms(200);
        
        I2C_Master_Start();
        I2C_Master_Write(0x50);   //Address del sensor TC74
        I2C_Master_Write(0x01); //Comando del sensor
        I2C_Master_Stop();
        __delay_ms(200);
        
        I2C_Master_Start();
        I2C_Master_Write(0x51);
        tem2 = I2C_Master_Read(0);
        I2C_Master_Stop();
        __delay_ms(200);
        
//        //Lectura Sensor SHT21
//        I2C_Master_Start();
//        I2C_Master_Write(0b10011010);   //Address del sensor TC74
//        I2C_Master_Write(0x00); //Comando del sensor
//        __delay_ms(100);
//        I2C_Master_Stop();
//        
//        I2C_Master_Start();
//        __delay_ms(100);
//        I2C_Master_Write(0b10011011);
//        __delay_ms(100);
//        sensor = I2C_Master_Read(0);
//        I2C_Master_Stop();
//        __delay_ms(200);
        
        
//        //Lectura Esclavo 2
//        I2C_Master_Start();
//        I2C_Master_Write(0x61);
//        contador = I2C_Master_Read(0);
//        I2C_Master_Stop();
//        __delay_ms(200);
        
        temp = tem1 << 8;
        temp = temp + tem2;
        
        volt = (temp/ (float) 1023)*5;
        temperatura = (volt* (float) 100);
        
        conv1 = 0;
        
        conv1 = temperatura;
        
        convert(lcd1, conv1, 2);//
        
        
        //Mostramos en el LCD los valores de los sensores
        Lcd_Set_Cursor(2, 1);             //Elegimos posicion
        Lcd_Write_String(lcd1);        //Escribimos valor del sensor
        Lcd_Set_Cursor(2, 6);             //Nueva posicion
        Lcd_Write_String("C");            //Dimensional del sensor
        
//        Lcd_Set_Cursor(2, 7);
//        Lcd_Write_String(lcd2);
//        Lcd_Set_Cursor(2, 11);
//        Lcd_Write_String("C");
//        
//        Lcd_Set_Cursor(2, 14);
//        Lcd_Write_String(lcd3);
        
        //Preparación de los sensores para ser mostrados en el LCD
        conv1 = 0;//se reinicia las cada ves que se inicia el proceso de enviar datos
        conv2 = 0;//tanto para la LCD como por UART.
        
//        conv1 = tem1;
//        conv1 = conv1 + tem2;
        
//        conv1 = (conv1/ (float)1023)*500;
        //maximo que un puerto puede tener, despues se multiplica por 5 para conocer el voltaje actual del puerto                                          
//        convert(lcd1, conv1, 2);//se convierte el valor actual a un valor ASCII.
        
//        conv2 = tem2;
//        PORTD = tem1;
//        convert(lcd2, conv2, 2);
        
//        temp = tem1 << 8;
//        temp = temp + tem2;
//        volt = (temp/(float)1023)*5;
//        temperatura = (volt*(float)100);
//        
//        conv3 = temp; 
//        convert(lcd1, volt, 2);
        
        __delay_ms(500);

    }
    return;
}
//*****************************************************************************
// Función de Inicialización
//*****************************************************************************
void setup(void){
    ANSEL = 0x00;
    ANSELH = 0x00;
    
    TRISCbits.TRISC0 = 0;
    TRISCbits.TRISC1 = 0;
    TRISB = 0x00;
    TRISD = 0x00;
    
    PORTB = 0x00;
    PORTD = 0x00;
    I2C_Master_Init(100000);        // Inicializar Comuncación I2C
    
    //Configuracion del Oscilador
    OSCCONbits.IRCF2 = 1;       //Reloj interno de 8MHz
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 1;
    OSCCONbits.SCS   = 1;
}