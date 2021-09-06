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
volatile uint8_t var_hum = 0;
float temp;
float volt, temperatura;

char lcd1[10];
char humedad[10];
char lcd3[10];

float conv1 = 0;
float conv0 = 0;
float conv3 = 0;

uint8_t datos_sensor[3];
int dato;

char cent, dect, unit, cenh, dech, unih;
char ingreso, posicion, ada;
//*****************************************************************************
// Definición de funciones para que se puedan colocar después del main de lo 
// contrario hay que colocarlos todas las funciones antes del main
//*****************************************************************************
void setup(void);
char centenas (int dato);
char decenas (int dato);
char unidades (int dato);
void USART_Tx(char data);
char USART_Rx(void);
void USART_Cadena(char *str);


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
        Lcd_Write_String("TEMP");
        Lcd_Set_Cursor(1, 8);
        Lcd_Write_String("HUM");
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
        
         I2C_Master_Start();
         I2C_Master_Write(0x11); //va a esclavo con direccion 0x10 y le dice que
                                 //va a leer
         var_hum = I2C_Master_Read(0);
         I2C_Master_Stop();
         __delay_ms(200);
        
        temp = tem1 << 8;
        temp = temp + tem2;
        
        volt = (temp/ (float) 1023)*5;
        temperatura = (volt* (float) 100);
        
        conv1 = 0;
        
        conv1 = temperatura;
        
        convert(lcd1, conv1, 2);//
        //*******************esta parte es del sensor de humedad****************
        conv0 = 0;//se reinicia las cada ves que se inicia el proceso de enviar datos
        conv0 = var_hum; //Se guarda el dato de humedad.     
        convert(humedad, conv0, 2);//se convierte el valor actual a un valor ASCII.
        
        //Mostramos en el LCD los valores de los sensores
        Lcd_Set_Cursor(2, 1);             //Elegimos posicion
        Lcd_Write_String(lcd1);        //Escribimos valor del sensor
        Lcd_Set_Cursor(2, 6);             //Nueva posicion
        Lcd_Write_String("C");            //Dimensional del sensor
        
        Lcd_Set_Cursor(2, 7);
        Lcd_Write_String(humedad);
        Lcd_Set_Cursor(2, 11);
        Lcd_Write_String("C");
        
        cent = centenas(temperatura); //Separamos el contador en centenas,
        dect = decenas(temperatura);  //decenas y unidades
        unit = unidades(temperatura);
        cent += 48;  //Convertimos a ASCII las variables
        dect += 48;
        unit += 48;
        
        cenh = centenas(humedad); //Separamos el contador en centenas,
        dech = decenas(humedad);  //decenas y unidades
        unih = unidades(humedad);
        cenh += 48;  //Convertimos a ASCII las variables
        dech += 48;
        unih += 48;
        
        if (PIR1bits.RCIF == 1){ //compruebo si se introdujo un dato
            ingreso = USART_Recieve();
            
            if(ingreso == 's'){
                USART_Tx(cent);  //temperatura
                USART_Tx(dect);
                USART_Tx(unit);
            }
            else if (ingreso == 'd') {
                USART_Tx(cenh);  //humedad
                USART_Tx(dech);
                USART_Tx(unih);
            }
            
        }
        ingreso = 0; //se limpia la variable una vez enviados los datos
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
    TRISE = 0x00;
    PORTE = 0x00;
    PORTB = 0x00;
    PORTD = 0x00;
    I2C_Master_Init(100000);        // Inicializar Comuncación I2C
    
    //*****************************conf UART************************************
    void init_USART (void);
    //**************************************************************************
    //*******************Configuracion del Oscilador****************************
    OSCCONbits.IRCF2 = 1;       //Reloj interno de 8MHz
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 1;
    OSCCONbits.SCS   = 1;
}

char centenas (int dato){
    char out = dato / 100;
    return out;
}

char decenas (int dato){
    char out;
    out = (dato % 100) / 10;
    return out;
}

char unidades (int dato){
    char out;
    out = (dato % 100) % 10;
    return out;
}

void USART_Tx(char data){       //envio de un caracter
    while(TXSTAbits.TRMT == 0);
    TXREG = data;
}

char USART_Rx(){                //Lectura de comunicacion serial
    return RCREG; 
   }

void USART_Cadena(char *str){   //Envio de cadena de caracteres
    while(*str != '\0'){
        USART_Tx(*str);
        str++;
    }
}