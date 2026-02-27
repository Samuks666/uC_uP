//==============================================================================
// Projeto:       Geracao de sinais 1 Hz e 10 Hz com Timer3
// Arquivo:       main.c
// Descrição:     Usa apenas o Timer3 para gerar dois sinais periodicos:
//                 - RD1: 1 Hz
//                 - RD0: 10 Hz
//                O programa principal inicializa e entra em loop infinito,
//                controle feito por interrupcoes
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
// Observacoes:
// - Timer3 configurado em modo 16 bits, prescaler 1:8
// - Tempo de overflow ? 5 ms (Fosc/4 = 12 MHz)
// - 1 Hz ? alterna a cada 100 interrupções
// - 10 Hz ? alterna a cada 10 interrupções
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
// Timers -> Fosc/4 = 12 MHz -> 83,33ns por incremento
// Prescaler = 1:8 -> 0,6667us por incremento
// Contagens (65536 - 58036) = 7500
// Tov (Tempo de overflow) = 7500 * 0,6667us = 5ms
//
// Para 1 Hz -> meio periodo = 0,5s -> 0,5/0,005 = 100 interrupcoes
// Para 10 Hz -> meio periodo = 0,05 -> 0,05/0,005 = 10 interrupcoes


#define TMR3_RELOAD  (65536 - 58036)
#define CNT_1HZ_TOGGLE  100
#define CNT_10HZ_TOGGLE 10

volatile unsigned char cnt_1Hz = 0;
volatile unsigned char cnt_10Hz = 0;

//==============================================================================
// INTERRUPCAO
//==============================================================================
void __interrupt(high_priority) high_isr(void)
{
    if (PIR2bits.TMR3IF)
    {
        PIR2bits.TMR3IF = 0;
        TMR3 = TMR3_RELOAD;

        // --- Saida de 1 Hz (RD1) ---
        cnt_1Hz++;
        if (cnt_1Hz >= CNT_1HZ_TOGGLE)
        {
            cnt_1Hz = 0;
            LATDbits.LATD1 = !LATDbits.LATD1;
        }

        // --- Saida de 10 Hz (RD0) ---
        cnt_10Hz++;
        if (cnt_10Hz >= CNT_10HZ_TOGGLE)
        {
            cnt_10Hz = 0;
            LATDbits.LATD0 = !LATDbits.LATD0;
        }
    }
}

void main(void)
{
    ADCON1 = 0x0F;  // Tudo digital
    CMCON = 0x07;   // Comparadores off

    TRISD = 0x00;   // PORTD como saida
    LATD = 0x00;

    // Configura Timer3 (16 bits, prescaler 1:8, clock interno)
    T3CONbits.TMR3CS = 0;     // Clock interno (Fosc/4)
    T3CONbits.T3CKPS = 0b11;  // Prescaler 1:8
    T3CONbits.RD16 = 1;       // Modo 16 bits
    TMR3 = TMR3_RELOAD;

    PIR2bits.TMR3IF = 0;
    PIE2bits.TMR3IE = 1;

    // Habilita interrupcoes com prioridade
    RCONbits.IPEN = 1;
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;

    // Liga Timer3
    T3CONbits.TMR3ON = 1;

    while (1)
    {

    }
}