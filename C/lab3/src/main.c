//==============================================================================
// Projeto:       Geracao de sinais 1 Hz e 10 Hz com Timer1 e Timer3
// Arquivo:       main.c
// Descrição:     Utiliza o Timer1 para gerar um sinal de 1 Hz no pino RD1 e o
//                Timer3 para gerar um sinal de 10 Hz no pino RD0. O programa
//                principal apenas inicializa o microcontrolador e entra em loop
//                infinito, enquanto as interrupções cuidam das saídas.
//
// Microcontrolador: PIC18F4550
// Oscilador:         Cristal externo de 20 MHz com PLL (F_CPU = 48 MHz)
// Compilador:        MPLAB XC8 v3.0
// Ferramenta:        MPLAB X IDE v5.15
//
// Autor:            Augusto H. S. Miranda e Samuel A. O. Rovida
// Data:             04/11/2025
// Versão:           v1.0
//
// Observações:
// - Timer1: 1 Hz (RD1) ? alterna a cada 100 interrupções (0,5 s)
// - Timer3: 10 Hz (RD0) ? alterna a cada 10 interrupções (0,05 s)
// - Ambos configurados em modo 16 bits com prescaler 1:8
// - Tempo de overflow ? 5 ms (Fosc/4 = 12 MHz)
// - Interrupções configuradas com prioridade habilitada
//==============================================================================


#include <xc.h>

//==============================================================================
// CONFIG BITS PIC18F4550
//==============================================================================

// CONFIG1L
#pragma config PLLDIV = 5        // Divide o clock do cristal por 5 (20 MHz / 5 = 4 MHz)
#pragma config CPUDIV = OSC1_PLL2// Clock do CPU vem do PLL (96MHz) dividido por 2 = 48MHz
#pragma config USBDIV = 2        // Clock do USB vem do PLL (96MHz) dividido por 2 = 48MHz

// CONFIG1H
#pragma config FOSC = HSPLL_HS   // Oscilador HS + PLL habilitado

// CONFIG2L
#pragma config PWRT = ON
#pragma config BOR = ON
#pragma config BORV = 3
#pragma config VREGEN = ON

// CONFIG2H
#pragma config WDT = OFF

// CONFIG3H
#pragma config MCLRE = ON
#pragma config LPT1OSC = OFF
#pragma config PBADEN = OFF

// CONFIG4L
#pragma config STVREN = ON
#pragma config LVP = OFF
#pragma config ICPRT = OFF
#pragma config XINST = OFF
#pragma config DEBUG = OFF

#define _XTAL_FREQ 48000000UL

// ============================================================================
// CALCULO DE TEMPORIZACAO
// ============================================================================
// Timers(Timer1 e Timer3) -> Fosc/4 = 12 MHz -> 83,33ns por incremento
// Prescaler = 1:8 -> 0,6667us por incremento
// Contagens (65536 - 58036) = 7500
// Tov (Tempo de overflow) = 7500 * 0,6667us = 5ms
//
// Para 1 Hz -> meio periodo = 0,5s -> 0,5/0,005 = 100 interrupcoes
// Para 10 Hz -> meio periodo = 0,05 -> 0,05/0,005 = 10 interrupcoes


#define TMR_RELOAD  (65536 - 58036)
#define TIMER1_TOGGLES  100
#define TIMER3_TOGGLES   10

volatile unsigned char count1 = 0;
volatile unsigned char count3 = 0;

//==============================================================================
// INTERRUPCAO
//==============================================================================
void __interrupt(high_priority) high_isr(void)
{
    // --- Timer1 (1 Hz em RD1) ---
    if (PIR1bits.TMR1IF)
    {
        PIR1bits.TMR1IF = 0;           // Limpa flag
        TMR1 = TMR_RELOAD;             // Recarrega

        count1++;
        if (count1 >= TIMER1_TOGGLES)
        {
            count1 = 0;
            LATDbits.LATD1 = !LATDbits.LATD1;   // Inverte RD1
        }
    }

    // --- Timer3 (10 Hz em RD0) ---
    if (PIR2bits.TMR3IF)
    {
        PIR2bits.TMR3IF = 0;           // Limpa flag
        TMR3 = TMR_RELOAD;             // Recarrega

        count3++;
        if (count3 >= TIMER3_TOGGLES)
        {
            count3 = 0;
            LATDbits.LATD0 = !LATDbits.LATD0;   // Inverte RD0
        }
    }
}

void main(void)
{
    ADCON1 = 0x0F;    // Tudo digital
    CMCON = 0x07;     // Comparadores desligados

    TRISD = 0x00;     // PORTD como saida
    LATD = 0x00;      // Inicializa pinos em 0

    // Configura Timer1 (16 bits, prescaler 1:8, clock interno)
    T1CONbits.TMR1CS = 0;  // Clock interno (Fosc/4)
    T1CONbits.T1CKPS = 0b11; // Prescaler 1:8
    T1CONbits.RD16 = 1;    // Modo 16 bits
    TMR1 = TMR_RELOAD;     // Valor inicial
    PIR1bits.TMR1IF = 0;   // Limpa flag
    PIE1bits.TMR1IE = 1;   // Habilita interrupcao do Timer1

    // Configura Timer3 (16 bits, prescaler 1:8, clock interno)
    T3CONbits.TMR3CS = 0;  // Clock interno
    T3CONbits.T3CKPS = 0b11; // Prescaler 1:8
    T3CONbits.RD16 = 1;
    TMR3 = TMR_RELOAD;
    PIR2bits.TMR3IF = 0;
    PIE2bits.TMR3IE = 1;

    // Habilita sistema de interrupcoes com prioridade
    RCONbits.IPEN = 1;     // Ativa niveis de prioridade
    INTCONbits.GIEH = 1;   // Habilita interrupcoes de alta prioridade
    INTCONbits.GIEL = 1;   // Habilita interrupcoes de baixa prioridade

    // Liga timers
    T1CONbits.TMR1ON = 1;
    T3CONbits.TMR3ON = 1;

    while (1)
    {

    }
}
