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

////----------------------------librerias a utilizar------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <xc.h>
#include <string.h> // Concatenar
#include "LCD.h"
#include "ADC.h"
#include "DHT11.h"


//*******************************definiciones***********************************
#define _XTAL_FREQ 8000000 
#define DHT11_PIN PORTAbits.RA0 
#define DHT11_PIN_Direction TRISAbits.TRISA0
//******************************************************************************
//  variables y prototipos
//******************************************************************************

uint8_t contador;
uint8_t Temp1;
uint8_t dummyT1;
uint8_t Hum1;
uint8_t dummyHum1;
uint8_t CHECKSUM;
char temperatura[10];
char humedad[10];
float conv0 = 0;
float conv1 = 0;
//************************************prototipos********************************
void config(void);
//******************************************************************************
//  funciones y loop principal
//******************************************************************************
void main(void){
    config();
    Lcd_Init();
    Lcd_Clear();
    while(1){
        
        DHT11_start();
        if(DHT11_response()){
            DHT11_ReadData(&Hum1);
            DHT11_ReadData(&dummyHum1);
            DHT11_ReadData(&Temp1);
            DHT11_ReadData(&dummyT1);
            DHT11_ReadData(&CHECKSUM);
            PORTD++;
            
            if(CHECKSUM == ((Hum1 + dummyHum1 + Temp1 + dummyT1) & 0XFF)){ 
                //se revisa si los datos que se pasaron fueron los correctos
                conv0 = 0;
                conv1 = 0;
                conv0 = Hum1; 
                conv1 = Temp1;
                convert(humedad, conv0, 2); //se convierte el valor actual a un valor ASCII.
                convert(temperatura, conv1, 2);
  
                Lcd_Set_Cursor(1, 1); //primeras cordenadas de la pantalla
                Lcd_Write_String("hum:"); //se escribe en la pantalla
                Lcd_Set_Cursor(1, 8); //nos despalazamos en la pantalla
                Lcd_Write_String("Temp:"); //se escribe de nuevo
                Lcd_Set_Cursor(2, 1);
                Lcd_Write_String(humedad);
                Lcd_Set_Cursor(2, 7);
                Lcd_Write_String(temperatura);
                //los datos pasaron exitsamentee
            }
            else{ //si el dato no es correcto se avisa al usuario
                Lcd_Clear();
                Lcd_Set_Cursor(1,1);
                Lcd_Write_String("error en datos");  
            }
        }
        __delay_ms(500);
    }
}

void config(void){
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
 
    
    //Configuracion del oscilador
    OSCCONbits.IRCF = 0b111; //oscilador a 8Mhz
    OSCCONbits.SCS = 0b1;
    
    //Configuracion del timmer1
    T1CON = 0X10; //periodo de 1MHz, ya que fuente es FOSC/4
    TMR1H = 0; //valores en 0 para el conteo de los periodos de pulso
    TMR1L = 0;
}