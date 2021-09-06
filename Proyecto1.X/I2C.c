/*
 * File            : I2C.c
 * Author          : Ligo George
 * Company         : electroSome
 * Project         : I2C Library for MPLAB XC8
 * Microcontroller : PIC 16F877A
 * Created on April 15, 2017, 5:59 PM
 * Link: https://electrosome.com/i2c-pic-microcontroller-mplab-xc8/
 * Modificada por: Pablo Mazariegos con la ayuda del auxiliar Gustavo Ordoñez 
 * Basado en Link: http://microcontroladores-mrelberni.com/i2c-pic-comunicacion-serial/
 */
#include "I2C.h"
#define _XTAL_FREQ 8000000
#include <xc.h>
//*****************************************************************************
// Función para inicializar I2C Maestro
//*****************************************************************************
void I2C_Master_Init(const unsigned long c)
{
    SSPCON = 0b00101000; //I2C master mode y SDA y SCL como fuentes del puerto
    SSPCON2 = 0;   //configuración default del pic
    SSPADD = (_XTAL_FREQ/(4*c))-1;//baud rate/velocidad de transmisión de datos
    SSPSTAT = 0;
    TRISCbits.TRISC3 = 1;   //se configuran SCL y SDA como inputs
    TRISCbits.TRISC4 = 1;
}
//*****************************************************************************
// Función de espera: mientras se esté iniciada una comunicación,
// esté habilitado una recepción, esté habilitado una parada
// esté habilitado un reinicio de la comunicación, esté iniciada
// una comunicación o se este transmitiendo, el IC2 PIC se esperará
// antes de realizar algún trabajo
//*****************************************************************************
void I2C_Master_Wait()
{
    while ((SSPSTAT & 0x04) || (SSPCON2 & 0x1F));
}
//*****************************************************************************
// Función de inicio de la comunicación I2C PIC
//*****************************************************************************
void I2C_Master_Start()
{
    I2C_Master_Wait();      //espera que se cumplan las condiciones adecuadas
    SSPCON2bits.SEN = 1;    //inicia la comunicación i2c
}
//*****************************************************************************
// Función de reinicio de la comunicación I2C PIC
//*****************************************************************************
void I2C_Master_RepeatedStart()
{
    I2C_Master_Wait();      //espera que se cumplan las condiciones adecuadas
    SSPCON2bits.RSEN = 1;   //reinicia la comunicación i2c
}
//*****************************************************************************
// Función de parada de la comunicación I2C PIC
//*****************************************************************************
void I2C_Master_Stop()
{
    I2C_Master_Wait();      //espera que se cumplan las condiciones adecuadas
    SSPCON2bits.PEN = 1;    //detener la comunicación i2c
}
//*****************************************************************************
//Función de transmisión de datos del maestro al esclavo
//esta función devolverá un 0 si el esclavo a recibido
//el dato
//*****************************************************************************
void I2C_Master_Write(unsigned d)
{
    I2C_Master_Wait();      //espera que se cumplan las condiciones adecuadas
    SSPBUF = d;             
}
//*****************************************************************************
//Función de recepción de datos enviados por el esclavo al maestro
//esta función es para leer los datos que están en el esclavo
//*****************************************************************************
unsigned short I2C_Master_Read(unsigned short a)
{
    unsigned short temp;
    I2C_Master_Wait();      //espera que se cumplan las condiciones adecuadas
    SSPCON2bits.RCEN = 1;
    I2C_Master_Wait();      //espera que se cumplan las condiciones adecuadas
    temp = SSPBUF;
    I2C_Master_Wait();      //espera que se cumplan las condiciones adecuadas
    if(a == 1){
        SSPCON2bits.ACKDT = 0;
    }else{
        SSPCON2bits.ACKDT = 1;
    }
    SSPCON2bits.ACKEN = 1;          // Iniciar sequencia de Acknowledge
    return temp;                    // Regresar valor del dato leído
}
//*****************************************************************************
// Función para inicializar I2C Esclavo
//*****************************************************************************
void I2C_Slave_Init(uint8_t address)
{ 
    SSPADD = address;
    SSPCON = 0x36;      // 0b00110110
    SSPSTAT = 0x80;     // 0b10000000
    SSPCON2 = 0x01;     // 0b00000001
    TRISC3 = 1;
    TRISC4 = 1;     //revisar que esto funcione
    GIE = 1;
    PEIE = 1;
    SSPIF = 0;
    SSPIE = 1;
}
//*****************************************************************************
void conf_osc(uint8_t frec){
    switch(frec){                   //31kHz
        case 0: 
            OSCCONbits.IRCF = 000;     
            break;   
        case 1:                     //125kHz
            OSCCONbits.IRCF = 001;     
            break;
        case 2:                     //250kHz
            OSCCONbits.IRCF = 010;     
            break;
        case 3:                     //500kHz
            OSCCONbits.IRCF = 011;     
            break;
        case 4:                     //1MHz
            OSCCONbits.IRCF = 100;     
            break;
        case 5:                     //2MHz
            OSCCONbits.IRCF = 101;     
            break;
        case 7:                     //8MHz
            OSCCONbits.IRCF = 111;     
            break;   
        default:                     //4MHz
            OSCCONbits.IRCF = 110;     
            break;            
    }
    OSCCONbits.SCS = 1; //se utiliza el reloj interno del sistema
}

//Autor electrosome
void Lcd_Port(char a) {
    PORTB = a;
}

void Lcd_Cmd(char a) {
    RS = 0; //Busy flag activada, el dato en el puerto se toma como comando
    Lcd_Port(a);
    EN = 1; // empieza lectura
    __delay_ms(4);
    EN = 0; //termina lectura
}

void Lcd_Clear(void) {
    Lcd_Cmd(0);
    Lcd_Cmd(1);
}

void Lcd_Set_Cursor(char a, char b) {
    char temp;
    if (a == 1) {
        temp = 0x80 + b - 1;
        Lcd_Cmd(temp);
    } else if (a == 2) {
        temp = 0xC0 + b - 1;
        Lcd_Cmd(temp);
    }
}

void Lcd_Init(void) {
    Lcd_Port(0x00);
    __delay_ms(20);
    Lcd_Cmd(0x30);
    __delay_ms(5);
    Lcd_Cmd(0x30);
    __delay_ms(11);
    Lcd_Cmd(0x30);
    /////////////////////////////////////////////////////
    Lcd_Cmd(0x38);
    Lcd_Cmd(0x0C);
    Lcd_Cmd(0x06);
}

void Lcd_Write_Char(char a) {
    RS = 1; //lectura y escritura habilitados
    Lcd_Port(a); //lo que se va a transferir
    EN = 1; //se inicia lectura y escritura
    __delay_us(10);
    EN = 0; //se termina la lecutra y escritura
}

void Lcd_Write_String(char *a) {
    int i;
    for (i = 0; a[i] != '\0'; i++)
        Lcd_Write_Char(a[i]);
}

void Lcd_Shift_Right(void) {
    Lcd_Cmd(0x01);
    Lcd_Cmd(0x0C);
}

void Lcd_Shift_Left(void) {
    Lcd_Cmd(0x01);
    Lcd_Cmd(0x08);
}
// Conversion de ADC 
void convert(char *data,float a, int place) //definition
{
     int temp=a;
     float x=0.0;
     int digits=0;
     int i=0,mu=1;
     int j=0;
     if(a<0)
     {
            a=a*-1;
            data[i]='-';
            i++;
      }
     //exponent component
     while(temp!=0)
     {
         temp=temp/10;
         digits++;          
     }
     while(digits!=0)
     {
         if(digits==1)mu=1;
         else  for(j=2;j<=digits;j++)mu=mu*10;
         
         x=a/mu;
         a=a-((int)x*mu);
         data[i]=0x30+((int)x);
         i++;
         digits--;
         mu=1;
     }
     //mantissa component
     data[i]='.';
     i++;
     digits=0;
     for(j=1;j<=place;j++)mu=mu*10;
     x=(a-(int)a)*mu; //shift places
     a=x;
     temp=a;
     x=0.0;
     mu=1;
     digits=place;
     while(digits!=0)
     {
         if(digits==1)mu=1;
         else  for(j=2;j<=digits;j++)mu=mu*10;
         
         x=a/mu;
         a=a-((int)x*mu);
         data[i]=0x30+((int)x);
         i++;
         digits--;
         mu=1;
     }   
     
    data[i]='\n';
}
    // Alistar configuracion de ADC
void start_adc(uint8_t frec, uint8_t isr, uint8_t Vref, uint8_t justRL) {
    ADCON0bits.ADON = 1;
    __delay_us(200);
    switch (frec) {
        case 1: // Fosc/2
            ADCON0bits.ADCS0 = 0;
            ADCON0bits.ADCS1 = 0;
            break;
        case 2: // Fosc/8
            ADCON0bits.ADCS0 = 1;
            ADCON0bits.ADCS1 = 0;
            break;
        case 3: // Fosc/32
            ADCON0bits.ADCS0 = 0;
            ADCON0bits.ADCS1 = 1;
            break;
        case 4: // FRC 
            ADCON0bits.ADCS0 = 1;
            ADCON0bits.ADCS1 = 1;
            break;
    }
    if (isr == 1) {
        INTCONbits.GIE   = 1;       //Activamos la interupcion del ADC
        INTCONbits.PEIE  = 1;
        PIE1bits.ADIE = 1; //Se habilita la interrupcion del ADC
        PIR1bits.ADIF = 0; //Se limpia la bandera del ADC
    }
    if (justRL == 0) {
        ADCON1bits.ADFM = 0; //se justifica a la izquierda
    } else {
        ADCON1bits.ADFM = 1; //se justifica a la derecha 
    }
    switch (Vref) {
        case 0: //Referencia en VDD y VSS
            ADCON1bits.VCFG0 = 0;
            ADCON1bits.VCFG1 = 0;
            break;
        case 1: //Referencia en Vref+ pin y VSS
            ADCON1bits.VCFG0 = 1;
            ADCON1bits.VCFG1 = 0;
            break;
        case 2: //Referencia en VDD y Vref- pin
            ADCON1bits.VCFG0 = 0;
            ADCON1bits.VCFG1 = 1;
            break;
        case 3: //Referencia en Vref+ pin y Vref- pin 
            ADCON1bits.VCFG0 = 1;
            ADCON1bits.VCFG1 = 1;
            break;
    }
}

// Empezar a colocar el valor del canal
void start_ch(uint8_t channel) { //Habilita los canales del ADC 
    switch (channel) {
        case 0: //Analogico en pin RA0
            ANSELbits.ANS0 = 1;
            break;
        case 1: //Analogico en pin RA1
            ANSELbits.ANS1 = 1;
            break;
        case 2: //Analogico en pin RA2
            ANSELbits.ANS2 = 1;
            break;
        case 3: //Analogico en pin RA3
            ANSELbits.ANS3 = 1;
            break;
        case 4: //Analogico en pin RA4
            ANSELbits.ANS4 = 1;
            break;
        case 5: //Analogico en pin RA5
            ANSELbits.ANS5 = 1;
            break;
        case 6: //Analogico en pin RA6
            ANSELbits.ANS6 = 1;
            break;
        case 7: //Analogico en pin RA7
            ANSELbits.ANS7 = 1;
            break;
        case 8: //Analogico en pin RB2
            ANSELHbits.ANS8 = 1;
            break;
        case 9: //Analogico en pin RB3
            ANSELHbits.ANS9 = 1;
            break;
        case 10: //Analogico en pin RB1
            ANSELHbits.ANS10 = 1;
            break;
        case 11: //Analogico en pin RB4
            ANSELHbits.ANS11 = 1;
            break;
        case 12: //Analogico en pin RB0
            ANSELHbits.ANS12 = 1;
            break;
        case 13: //Analogico en pin RB5
            ANSELHbits.ANS13 = 1;
            break;
    }
}


// Seleccionar Canal de Conversion
void Select_ch(uint8_t channel) {// se selecciona el canal analogico y se inicia la conversion
    switch (channel) {
        case 0: // AN0
            ADCON0bits.CHS0 = 0;
            ADCON0bits.CHS1 = 0;
            ADCON0bits.CHS2 = 0;
            ADCON0bits.CHS3 = 0;
            break;
        case 1: // AN1
            ADCON0bits.CHS0 = 1;
            ADCON0bits.CHS1 = 0;
            ADCON0bits.CHS2 = 0;
            ADCON0bits.CHS3 = 0;
            break;
        case 2: // AN2
            ADCON0bits.CHS0 = 0;
            ADCON0bits.CHS1 = 1;
            ADCON0bits.CHS2 = 0;
            ADCON0bits.CHS3 = 0;
            break;
        case 3: // AN3
            ADCON0bits.CHS0 = 1;
            ADCON0bits.CHS1 = 1;
            ADCON0bits.CHS2 = 0;
            ADCON0bits.CHS3 = 0;
            break;
        case 4: // AN4
            ADCON0bits.CHS0 = 0;
            ADCON0bits.CHS1 = 0;
            ADCON0bits.CHS2 = 1;
            ADCON0bits.CHS3 = 0;
            break;
        case 5: // AN5
            ADCON0bits.CHS0 = 1;
            ADCON0bits.CHS1 = 0;
            ADCON0bits.CHS2 = 1;
            ADCON0bits.CHS3 = 0;
            break;
        case 6: // AN6
            ADCON0bits.CHS0 = 0;
            ADCON0bits.CHS1 = 1;
            ADCON0bits.CHS2 = 1;
            ADCON0bits.CHS3 = 0;
            break;
        case 7: // AN7
            ADCON0bits.CHS0 = 1;
            ADCON0bits.CHS1 = 1;
            ADCON0bits.CHS2 = 1;
            ADCON0bits.CHS3 = 0;
            break;
        case 8: // AN8
            ADCON0bits.CHS0 = 0;
            ADCON0bits.CHS1 = 0;
            ADCON0bits.CHS2 = 0;
            ADCON0bits.CHS3 = 1;
            break;
        case 9: // AN9
            ADCON0bits.CHS0 = 1;
            ADCON0bits.CHS1 = 0;
            ADCON0bits.CHS2 = 0;
            ADCON0bits.CHS3 = 1;
            break;
        case 10: // AN10
            ADCON0bits.CHS0 = 0;
            ADCON0bits.CHS1 = 1;
            ADCON0bits.CHS2 = 0;
            ADCON0bits.CHS3 = 1;
            break;
        case 11: // AN11
            ADCON0bits.CHS0 = 1;
            ADCON0bits.CHS1 = 1;
            ADCON0bits.CHS2 = 0;
            ADCON0bits.CHS3 = 1;
            break;
        case 12: // AN12
            ADCON0bits.CHS0 = 0;
            ADCON0bits.CHS1 = 0;
            ADCON0bits.CHS2 = 1;
            ADCON0bits.CHS3 = 1;
            break;
        case 13: // AN13
            ADCON0bits.CHS0 = 1;
            ADCON0bits.CHS1 = 0;
            ADCON0bits.CHS2 = 1;
            ADCON0bits.CHS3 = 1;
            break;
        case 14: // CVref
            ADCON0bits.CHS0 = 0;
            ADCON0bits.CHS1 = 1;
            ADCON0bits.CHS2 = 1;
            ADCON0bits.CHS3 = 1;
            break;
        case 15: // Fixed ref
            ADCON0bits.CHS0 = 1;
            ADCON0bits.CHS1 = 1;
            ADCON0bits.CHS2 = 1;
            ADCON0bits.CHS3 = 1;
            break;
    }
    __delay_us(200); //es el tiempo recomendado de espera por cada cambio de canal
    ADCON0bits.GO = 1;
}