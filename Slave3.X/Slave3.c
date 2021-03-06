// Archivo:  Slave3.c
// Dispositivo:	PIC16F887
// Autor:    Fernando Arribas
// Compilador:	pic-as (v2.31), MPLABX V5.45
// 
// Programa: Comunicacion I2C y sensores
//           
// Hardware: Leds en PORTD y push butons en PORTB
//           
//
// Creado: 31 aug, 2021
// Ultima modificacion: 31 aug, 2021
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

//*****************************************************************************
// Definición e importación de librerías
//*****************************************************************************
#include <pic16f887.h>
#include <xc.h>
#include <stdint.h>
#include "Librerias.h"

//*****************************************************************************
// Variables
//*****************************************************************************
//char val_temp;
uint16_t lec1; 
uint8_t tem1, tem2;

uint8_t counter, speed; 

//volatile uint8_t adc1 = 0;
//volatile uint8_t adc2 = 0;
//volatile uint8_t contador = 0;
//
//char temperatura[10];
//char sensor2[10];
//char cont1[10];
//
//float conv1 = 0;
//float conv2 = 0;
//float conv3 = 0;

uint8_t z;
uint8_t lec;

//*****************************************************************************
// Definiciones
//*****************************************************************************
#define _XTAL_FREQ 8000000

//*****************************************************************************
// Prototipos de Funciones
//*****************************************************************************
void setup(void);

//*****************************************************************************
// Código de Interrupción 
//*****************************************************************************
void __interrupt() isr(void)
{
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
            BF = 0;
            SSPBUF = 16;
//                if (lec == 1) {
//                    SSPBUF = tem1;
//                    PORTB++;
//                }
//                else if (lec == 2) {
//                    SSPBUF = 16;
//                }
                    
            SSPCONbits.CKP = 1;
            __delay_us(250);
            while(SSPSTATbits.BF);
        }
       
        PIR1bits.SSPIF = 0;    
    }
    
    //Interupcion del Timer 0 (Multiplexado)
    
    if (PIR1bits.ADIF) {
        if  (ADCON0bits.CHS == 0) { //Verificamos el canal que se esta convirtiendo
            lec1 = ADRESH << 8;         //Dependiendo el canal guardamos el resultado
            lec1 = lec1 + ADRESL;
            tem1 = ADRESH;
            tem2 = ADRESL;
        }
        
        else {
            PORTD = ADRESH;
        }
        
        PIR1bits.ADIF = 0;          //Reiniciamos la interupcion
    }
    
    if (INTCONbits.T0IF ==1) 
    {
        counter++;  //Creamos un contador que tenga de 0 a 255 un T = 1 seg
        INTCONbits.T0IF = 0;         // Limpiamos la bandera
        TMR0 = 131;           //reseteamos el timer
        
        //Proseguimos con el bit banging de los DC
        if (counter >= speed) { //Comparamos el valor del ADC con el contador
            PORTDbits.RD0 = 0;  //Si el contador es mayor los puertos son 0
        }
        
        else {                  //Si es menor los puertos son 1
            PORTDbits.RD0 = 1;
        }
        
        if (counter == 256) {   //Reiniciamos el contador
            counter = 0;
        }
        
    }
    
}

//*****************************************************************************
// Main
//*****************************************************************************
void main(void) {
    setup();                //Llamamos a la configuracion del PIC
//    unsigned int a;
//    Lcd_Init();
//    Lcd_Clear();
    ADCON0bits.GO   = 1;    //Damos inicio a la conversion
    
//******************************************************************************
//Loop principal
//******************************************************************************
    while(1){
        
//        //Primera linea del LDC
//        Lcd_Set_Cursor(1, 1);
//        Lcd_Write_String("Temp");
//        Lcd_Set_Cursor(1, 8);
//        Lcd_Write_String("S2:");
//        Lcd_Set_Cursor(1, 14);
//        Lcd_Write_String("S3:");
//        
//        volt1 = (lec1/ (float) 1023)*5;
//        sen_temp = (volt1* (float) 100);
        
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
        
//        //Mostramos en el LCD los valores de los sensores
//        Lcd_Set_Cursor(2, 1);             //Elegimos posicion
//        Lcd_Write_String(temperatura);        //Escribimos valor del sensor
//        Lcd_Set_Cursor(2, 6);             //Nueva posicion
//        Lcd_Write_String("C");            //Dimensional del sensor
        
        
//        //Preparación de los sensores para ser mostrados en el LCD
//        conv1 = 0;//se reinicia las cada ves que se inicia el proceso de enviar datos
//        conv2 = 0;//tanto para la LCD como por UART.
//        
//        conv1 = sen_temp;
//        //maximo que un puerto puede tener, despues se multiplica por 5 para conocer el voltaje actual del puerto                                          
//        convert(temperatura, conv1, 2);//se convierte el valor actual a un valor ASCII.
        
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
        
    }
    return;
    }

void setup(void) {
    //Configuracion de los puertos
    ANSEL   = 0X03;       //Colocamos RA0 y RA1 como entradas analogicas
    ANSELH  = 0X00;       
    
    TRISA   = 0X03;       //Colocamos RA0 y RA1 como entradas y el resto del
    TRISB   = 0X00;       //PORTA, el PORTB, PORTC y PORTD como salidas
    TRISC   = 0X00;
    TRISD   = 0X00;
    
    PORTA   = 0X00;
    PORTB   = 0X00;
    PORTC   = 0X00;
    PORTD   = 0X00;
    I2C_Slave_Init(0x50);
    
    //Configuracion del Oscilador
    OSCCONbits.IRCF2 = 1;       //Reloj interno de 8MHz
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 1;
    OSCCONbits.SCS   = 1;
            
    //Configuracion Interupciones
    INTCONbits.GIE   = 1;       //Activamos las interupciones ADC y del TMR0
    INTCONbits.PEIE  = 1;
    PIE1bits.ADIE    = 1;
    INTCONbits.T0IE  = 1;
    PIE1bits.SSPIE = 1;
    
    PIR1bits.ADIF    = 0;
    INTCONbits.T0IF  = 0;
    PIR1bits.SSPIF = 0;
    
    //Configuracion ADC
    ADCON1bits.ADFM     = 1;    //Justificado a la derecha
    ADCON1bits.VCFG0    = 0;    //Colocamos los voltajes de ref como VSS y VDD
    ADCON1bits.VCFG1    = 0;
    
    ADCON0bits.ADCS1    = 1;    //Reloj de conversion como FOSC/32
    ADCON0bits.CHS      = 0;    //Chanel 0
    __delay_us(200);
    ADCON0bits.ADON     = 1;    //Encendemos el ADC
    __delay_us(200);

    //Configuracion TMR0
    //Timer0 Registers Prescaler= 64 - TMR0 Preset = 131 - Freq = 250.00 Hz - Period = 0.004000 seconds
    OPTION_REGbits.T0CS = 0;  // bit 5  TMR0 Clock Source Select bit...0 = Internal Clock (CLKO) 1 = Transition on T0CKI pin
    OPTION_REGbits.T0SE = 0;  // bit 4 TMR0 Source Edge Select bit 0 = low/high 1 = high/low
    OPTION_REGbits.PSA = 0;   // bit 3  Prescaler Assignment bit...0 = Prescaler is assigned to the Timer0
    OPTION_REGbits.PS2 = 1;   // bits 2-0  PS2:PS0: Prescaler Rate Select bits
    OPTION_REGbits.PS1 = 0;
    OPTION_REGbits.PS0 = 1;
    TMR0 = 131;               // preset for timer register
}