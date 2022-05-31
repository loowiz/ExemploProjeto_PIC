//============================================================================//
//  Projeto: LCD - SENAI CFP 118
//============================================================================//

//============================================================================//
//  Includes
//============================================================================//
#include <xc.h>

#define _XTAL_FREQ  4000000

//============================================================================//
//  Definições de Hardware
//============================================================================//
#define RS          PORTBbits.RB2
//#define RW        [Endereço físico]
#define EN      	PORTBbits.RB3
#define DB4_LCD		PORTBbits.RB4	//Saída para o DB4 do LCD
#define DB5_LCD		PORTBbits.RB5	//Saída para o DB5 do LCD
#define DB6_LCD		PORTBbits.RB6	//Saída para o DB6 do LCD
#define DB7_LCD		PORTBbits.RB7	//Saída para o DB6 do LCD


//============================================================================//
//  Prototipagem das Funções
//============================================================================//
void IniciaLCD (void);
void ComandoLCD (unsigned char dado);
void EscreveLCD (unsigned char dado); //caracter
void StringLCD (const unsigned char *string); //texto
void NumeroLCD (unsigned int numero);// já separa o valor que está dentro da variável e escreve no LCD
void PosicionaLCD (unsigned char linha, unsigned char coluna);
void LimpaLCD (void);

//============================================================================//
//  IniciaLCD
//============================================================================//
void IniciaLCD (void)
{
    unsigned char i;
    unsigned char inicio[6] = {0x33, 0x32, 0x28, 0x0C, 0x06, 0x01};
    
    PORTB = PORTB & 0B00001111;
    RS = 0;
    EN = 0;
    //RW = 0;
    __delay_ms(15);
    //Delay1KTCYx(15);
    
    for (i = 0; i < 6; i++) ComandoLCD (inicio[i]);
}

//============================================================================//
//  ComandoLCD
//============================================================================//
void ComandoLCD (unsigned char dado)
{
    unsigned char i;
    
    for (i = 0; i < 2; i++)
    {
        if (i) PORTB = ((PORTB & 0B00001111) | (dado <<4));
        else PORTB = ((PORTB & 0B00001111) | (dado));
        
        RS = 0;
        EN = 1;
        __delay_us(50);
        //Delay10TCYx(5);
        EN = 0;
    }
    
    if ((dado == 0x01) || (dado == 0x02)) __delay_ms(2);//Delay1KTCYx(2);
    else __delay_us(50);//Delay10TCYx(5);
}

//============================================================================//
//  EscreveLCD
//============================================================================//

void EscreveLCD (unsigned char dado)
{
    unsigned char i;
    
    for (i = 0; i < 2; i++)
    {
        if (i) PORTB = ((PORTB & 0B00001111) | (dado <<4));
        else PORTB = ((PORTB & 0B00001111) | (dado));
        
        RS = 1;
        EN = 1;
        __delay_us(50);
        //Delay10TCYx(5);
        EN = 0;
    }
    
    __delay_us(50);
    //Delay10TCYx(5);
}

//============================================================================//
//  StringLCD
//============================================================================//

void StringLCD (const unsigned char *string)
{
    while (*string != 0)
    {
        EscreveLCD (*string);
        string++;
    }
}

//============================================================================//
//  NumeroLCD
//============================================================================//

void NumeroLCD (unsigned int numero)
{
    unsigned char i;
    
    if (numero >= 10000)
    {
        i = (numero / 10000) + 0x30;
        EscreveLCD (i);
    }
    
    if (numero >= 1000)
    {
        i = ((numero / 1000) % 10) + 0x30;
        EscreveLCD (i);
    }
    
    if (numero >= 100)
    {
        i = ((numero / 100) % 10) + 0x30;
        EscreveLCD (i);
    }
    
    if (numero >= 10)
    {
        i = ((numero / 10) % 10) + 0x30;
        EscreveLCD (i);
    }
    
    i = (numero % 10) + 0x30;
    EscreveLCD (i);
}

//============================================================================//
//  PosicionaLCD
//============================================================================//

void PosicionaLCD (unsigned char linha, unsigned char coluna)
{
    unsigned char posicao;
    coluna--;
    
    switch (linha)
    {
        case 1: posicao = 0x80 + coluna; break;
        case 2: posicao = 0xC0 + coluna; break;
        case 3: posicao = 0x94 + coluna; break;
        case 4: posicao = 0xD4 + coluna; break;
    }
    
    ComandoLCD (posicao);
}

//============================================================================//
//  LimpaLCD
//============================================================================//

void LimpaLCD (void)
{
    ComandoLCD (0x01);
}
