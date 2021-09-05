/* 
 * File:   LCD.c
 * Author: pablo
 *
 * Created on 25 de julio de 2021, 20:41
 */

//LCD Functions Developed by electroSome
#define _XTAL_FREQ 8000000
#include <xc.h>
#include "LCD.h"
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

