/* 
 * File:   Master_proyecto1.c
 * Author: pablo
 *
 * Created on 23 de agosto de 2021, 19:39
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
#include "I2C.h"
//*******************************definiciones***********************************
#define _XTAL_FREQ 8000000 
#define Sample_Rate_Divider 0x19
#define CONFIG_MPU6050 0x1A
#define Gyro_CONFIG 0x1B
#define Accel_CONFIG 0x1C
#define Power_Managment_1 0x6B
#define Accel_Xout_H 0x3B
#define MPU_Write 0xD0
#define MPU_Read 0xD1

//**********Variables***********
volatile uint8_t var_hum = 0;
float cont_uart = 0;
char string_uart[10];
char valor_uart = 0;
char adc0[10];
char contador_lcd[10];
float conv0 = 0;
uint8_t contador;
int8_t sensor;
char humedad[10];
float conv0 = 0;
 //**********Prototipos*********
 void setup(void);
 void I2C_MPU_Init(void);
 void I2C_Read_MPU(float* data_send);
 //**********Interrupcion********* 
//********************************loop principl*********************************
void main(void) {
    setup(); 
    Lcd_Init(); //se inisiliza la LCD  
    Lcd_Clear(); //se limpia la LCD
    void I2C_MPU_Init(void);
    while (1) {
         
        Lcd_Set_Cursor(1, 1); //primeras cordenadas de la pantalla
        Lcd_Write_String("Hum:"); //se escribe en la pantalla

        //****************se hace la cmunicación I2C con esclavo 1************** 
         I2C_Master_Start();
         I2C_Master_Write(0x11); //va a esclavo con direccion 0x10 y le dice que
                                 //va a leer
         var_hum = I2C_Master_Read(0);
         I2C_Master_Stop();
         __delay_us(200);
         
        //*********************se escribe en la LCD*****************************
        Lcd_Set_Cursor(2, 1); //nos colocamos abajo de V1 
        Lcd_Write_String(humedad); //mandamos el valor de la conversion 
        Lcd_Set_Cursor(2, 5);
        Lcd_Write_String("°C");
        
        conv0 = 0;//se reinicia las cada ves que se inicia el proceso de enviar datos
        conv0 = var_hum; //Se guarda el dato de humedad.                                         
        convert(humedad, conv0, 2);//se convierte el valor actual a un valor ASCII.
        
        }      
    return;
}
//********************************configuración*********************************
void setup(void){
    ANSEL =     0X00;
    ANSELH =    0X00;
    TRISA =     0X00;
    TRISB =     0X00; 
    TRISD =     0X00;
    TRISE =     0x00;
    PORTA =     0X00;
    PORTB =     0X00;
    PORTD =     0X00;
    PORTE =     0x00;
    
    I2C_Master_Init(100000);        // Inicializar Comuncación I2C
    //Configuracion del oscilador
    OSCCONbits.IRCF = 0b111; //oscilador a 8Mhz
    OSCCONbits.SCS = 0b1;
}
