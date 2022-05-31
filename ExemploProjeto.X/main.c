/*=======================================================
					SENAI A. Jacob Lafer
					====================

Nome do projeto:	Exemplo de projeto
Versão:				1.0x
Última revisão:		01/06/2020
Cliente:			SENAI
Desenvolvimento:	Luiz Ricardo Bitencourt

Descritivo Funcional:
 * Projeto de controle de um forno industrial

Inputs:
- RB0 e RB1 - Chaves
- RC2 e RC4 - Chaves
 
Outputs:
- RD0-RD3 - LEDs
- RB2 - Display RS
- RB3 - Display E
- RB4-RB7 - Display Data
=========================================================*/

//======================================================
// Bibliotecas
//======================================================
#include <xc.h>
#include "lcd_4vias.h"
#include "adc.h"
#include "pwm.h"
#include "timer0.h"

// =====================================================
// CONFIGURAÇÕES DO MICROCONTROLADOR
// =====================================================
#pragma config  FOSC    = HS
#pragma config  PLLDIV  = 1
#pragma config  CPUDIV  = OSC1_PLL2
#pragma config  USBDIV  = 1
#pragma config  BORV    = 0
#pragma config  WDTPS   = 1
#pragma config  MCLRE   = ON
#pragma config	PWRT=ON, LVP=OFF, IESO=OFF, WDT=OFF, PBADEN=OFF, BOR=OFF
#pragma config  CCP2MX=ON, LPT1OSC=OFF, ICPRT=OFF, XINST=OFF, VREGEN=OFF
#pragma config  CP0=OFF, CP1=OFF, CP2=OFF, CP3=OFF, CPB=OFF, CPD=OFF, WRTD = OFF
#pragma config  WRT0=OFF, WRT1=OFF, WRT2=OFF, WRT3=OFF, WRTC = OFF, WRTB = OFF
#pragma config  EBTR0=OFF, EBTR1=OFF, EBTR2=OFF, EBTR3=OFF, EBTRB=OFF
// =====================================================

//======================================================
// Definições de Hardware
//======================================================
#define Esteira     PORTDbits.RD1
#define Estufa      PORTDbits.RD2
#define Sensor_S1   PORTCbits.RC4
#define Sensor_S2   PORTCbits.RC2
#define Sensor_S3   PORTBbits.RB0
#define Emergencia  PORTBbits.RB1

#define _XTAL_FREQ  4000000
//======================================================

//======================================================
// Declaração de Variáveis
//======================================================
char    Estado = 1;         // Variável usada na máquina de estados
int				Cronometro  = 0,
                Segundos	= 0,
                Minutos     = 0,
                Horas       = 0,
		 		Preset		= 0xc2f7;  // Período de 1s
//======================================================

//======================================================
// Interrupções
//======================================================
void __interrupt() high_isr(void) {     // Tratamento de interrupção de alta prioridade (ISR)
    if (INTCONbits.TMR0IF) {            // Verifica se overflow TMR0IF = 1 (garante que não executará sem ter estourado a contagem)
        INTCONbits.TMR0IF = 0;          // Zera flag de overflow
                                        // Recarga do preset do Timer 0:
        TMR0H = Preset >> 8;            // Registrador de contagem do Timer 0 - Parte alta
        TMR0L = Preset;                 // Registrador de contagem do Timer 0 - Parte baixa

        Cronometro++;
        Segundos++;
        if (Segundos == 60) {
            Segundos = 0;
            Minutos++;
            if (Minutos == 60) {
                Minutos = 0;
                Horas++;
                if (Horas == 24) {
                    Horas = 0;
                }
            }
        }
    }
}
//======================================================

//======================================================
// Programa Principal
//======================================================
void main (void){
    // -------------------------------------------------
	// Criação das variáveis locais
	// -------------------------------------------------
    int ValorTemp;
    int i;
    int Tempo;
    // -------------------------------------------------
    
	// -------------------------------------------------
	// Configurações iniciais
	// -------------------------------------------------
	TRISDbits.TRISD1 = 0;   // D1 é uma saída (aciona Esteira)
    TRISDbits.TRISD2 = 0;   // D2 é uma saída (aciona Estufa)
    TRISB = 0b00000011;     // B0 e B1 são entradas, demais são saídas
    TRISCbits.TRISC2 = 1;   // C2 é uma entrada
    //TRISCbits.RC4 = 1;    // Apenas para mostrar que o pino RC4 não é configurado como entrada desta forma no 18F4550
                            // RC4 é ativada pelas opções de USB abaixo.
    UCON = 0x00;            //** PARA O PIC 18F4550: desativa opções de USB para utilizar a porta RC4 como entrada
    UCFGbits.UTRDIS = 1;    //** PARA O PIC 18F4550: desativa opções de USB para utilizar a porta RC4 como entrada
    
    INTCON2bits.NOT_RBPU = 1; // Desliga os pull-ups internos do PORT B
    
	IniciaLCD();            // Inicializa LCD
    Inicializa_PWM(255);    // Inicializa PWM
    Inicializa_ADC();       // Inicializa ADC
    Inicializa_Timer0(Timer0_INT_ON, Preset);	// Inicializa Timer0 com interrupção e Preset
	// -------------------------------------------------

	// -------------------------------------------------
	// Programa principal
	// -------------------------------------------------
	PosicionaLCD(1,1);
    StringLCD("Estufa Plus v1.0");
    PosicionaLCD(2,1);
    
    // Início da estrutura da máquina de estados
    while(1){
        switch(Estado){
            case 1:
                // -------------------
                // Ações do estado 1:
                // -------------------
                Esteira = 0;        // Mantém esteira desligada
                Estufa = 0;         // Mantém estufa desligada
                DutyCycle_PWM(0);   // Mantém ventilador desligado
                PosicionaLCD(2,1);
                StringLCD("Inserir Produto!");
                // -------------------
                // Condição de transição:
                // -------------------
                if(Sensor_S1 == 1){ // Se inserir produto
                    Estado = 2;     // pula para o estado 2
                }
                // -------------------
                break;
                
            case 2:
                // -------------------
                // Ações do estado 2:
                // -------------------
                Esteira = 1;        // Liga esteira
                Estufa = 0;         // Mantém estufa desligada
                DutyCycle_PWM(0);   // Mantém ventilador desligado
                PosicionaLCD(2,1);
                StringLCD("Deslocando...   ");
                // -------------------
                // Condição de transição:
                // -------------------
                if(Sensor_S2 == 1){ // Se chegar próximo da esteira
                    Estado = 3;     // pula para o estado 3
                }
                // -------------------
                break;
                
            case 3:
                // -------------------
                // Ações do estado 3:
                // -------------------
                Esteira = 1;        // Mantém esteira ligada
                DutyCycle_PWM(0);   // Mantém ventilador desligado
                
                PosicionaLCD(2,1);
                StringLCD("Temp.:          ");
                
                ValorTemp = ((LerADC()*5.0)/1024)*20;
                PosicionaLCD(2,8);
                EscreveLCD((ValorTemp/10)%10 + 48);
                EscreveLCD(ValorTemp%10 + 48);
                StringLCD(" graus");
                __delay_ms(100);
                
                if(ValorTemp < 55){
                    Estufa = 1;         // Liga estufa (temperatura menor que 55 graus)
                }
                if(ValorTemp > 65){
                    Estufa = 0;         // Desliga estufa (temperatura maior que 65 graus)
                }
                // -------------------
                // Condição de transição:
                // -------------------
                if(Sensor_S3 == 1){ // Se chegar no fim do processo
                    Estado = 4;     // pula para o estado 4
                }
                // -------------------
                break;
                
            case 4:
                // -------------------
                // Ações do estado 4:
                // -------------------
                Esteira = 0;        // Desliga esteira
                Estufa = 0;         // Desliga estufa
                
                PosicionaLCD(2,1);
                StringLCD("Ligando ventil. ");
                
                for(i=0;i<1000;i=i+10){
                    DutyCycle_PWM(i);   // Aciona gradualmente o ventilador  
                    __delay_ms(10);
                }
                Tempo = Cronometro;
                // -------------------
                // Condição de transição:
                // -------------------
                Estado = 5;     // A condição é ter realizado as ações anteriores
                // -------------------
                break;

            case 5:
                // -------------------
                // Ações do estado 5:
                // -------------------
                Esteira = 0;        // Mantém esteira desligada
                Estufa = 0;         // Mantém estufa desligada
                DutyCycle_PWM(1000);   // Mantém ventilador em rotação máxima
                
                PosicionaLCD(2,1);
                StringLCD("Resfriando...   ");
                
                // -------------------
                // Condição de transição:
                // -------------------
                if(Cronometro >= (Tempo + 5)){   // Passados 30s
                    Estado = 6;     // pula para o estado 6
                }
                // -------------------
                break;

            case 6:
                // -------------------
                // Ações do estado 6:
                // -------------------
                Esteira = 0;        // Mantém esteira desligada
                Estufa = 0;         // Mantém estufa desligada
                
                PosicionaLCD(2,1);
                StringLCD("Finalizando...");
                
                for(i=1000;i>0;i=i-10){
                    DutyCycle_PWM(i);   // Desliga gradualmente o ventilador  
                    __delay_ms(10);
                }

                // -------------------
                // Condição de transição:
                // -------------------
                Estado = 1;     // Fim do processo, pula para o estado 1
                // -------------------
                break;
        }
    }
    // Fim da estrutura da máquina de estados
	// -------------------------------------------------
}
//======================================================

