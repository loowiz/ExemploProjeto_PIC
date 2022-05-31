#ifndef	PWM_H_
#define PWM_H_

// ============================================================================================ //
//							BIBLIOTECA PARA USO DE PWM NO PIC 18F4550							//
//																								//
//	ESCOLA SENAI "A. JACOB LAFER																//
//	CURSO T�CNICO DE ELETROELETR�NICA															//
//	DISCIPLINA: DESET																			//
//	DATA: 21/09/2016																			//
//	REVIS�O: B (KIT PIC	CT EaD)																	//
// ============================================================================================	//

/*
Seguindo o Datasheet (p.147)
----------------------------
15.4.4 SETUP FOR PWM OPERATION
1. Set the PWM period by writing to the PR2 register.
2. Set the PWM duty cycle by writing to the CCPRxL register and CCPxCON<5:4> bits.
3. Make the CCPx pin an output by clearing the appropriate TRIS bit.
4. Set the TMR2 prescale value, then enable Timer2 by writing to T2CON.
5. Configure the CCPx module for PWM operation.
*/

// -----------------------------------------------------
// Rotina de inicializa��o do m�dulo PWM
// -----------------------------------------------------
// Para c�lculo do per�odo do PWM (p. 153 do datasheet):
// Periodo = [(PR2) + 1] * 4 * Tosc * (Timer2 Prescaler)
// -----------------------------------------------------
void Inicializa_PWM(unsigned char Valor) {
    PR2 = Valor;            // Registrador que cont�m o per�odo do Timer2 (p. 137 do datasheet)
    TRISCbits.TRISC1 = 0;   // Usando o m�dulo CCP2 (pino C1)
    T2CON = 0;              // Timer2 desligado, Prescaler = 1 (p. 137 do datasheet)
    TMR2ON = 1;
    PR2 = 0xFF;             // Registrador que cont�m o per�odo do Timer2 (p. 137 do datasheet)
    CCP1CONbits.P1M1 = 1;   // (p. 149 do datasheet)
    CCP1CONbits.P1M0 = 0;    
    CCP2CON = 0b00001100;   // PWM com uma sa�da (p. 151 do datasheet)
}
// -----------------------------------------------------


// -----------------------------------------------------
// Rotina de configura��o de Duty Cycle do PWM
// -----------------------------------------------------
// Para c�lculo do Duty Cycle do PWM (p.154 do datasheet):
// DutyCycle = (CCPR2L:CCP2CON<5:4>) * TOSC * (TMR2 Prescale Value)
// -----------------------------------------------------
void DutyCycle_PWM(unsigned int Valor) {
    CCP2CONbits.DC2B0 = Valor;
    CCP2CONbits.DC2B1 = Valor >> 1;
    CCPR2L = Valor >> 2;
}
// -----------------------------------------------------

#endif