#include <xc.h>
#include <stdio.h>
#include "xlcd.h"

// CONFIG1L
#pragma config PLLDIV = 5        // Divide o clock do cristal por 5 (20 MHz / 5 = 4 MHz)
#pragma config CPUDIV = OSC1_PLL2// Clock do CPU vem do PLL (96MHz) dividido por 2 = 48MHz
#pragma config USBDIV = 2        // Clock do USB vem do PLL (96MHz) dividido por 2 = 48MHz

// CONFIG1H
#pragma config FOSC = HSPLL_HS   // Usa o oscilador HS (High Speed) com o PLL habilitado

// CONFIG2L
#pragma config PWRT = ON         // Power-up Timer habilitado (estabiliza a alimentacao na partida)
#pragma config BOR = ON          // Brown-out Reset habilitado (reseta o PIC se a tensao cair)
#pragma config BORV = 3          // Nivel de Brown-out Reset em 2.1V
#pragma config VREGEN = ON       // Regulador de tensao interno do USB habilitado (essencial se usar USB)

// CONFIG2H
#pragma config WDT = OFF         // Watchdog Timer DESLIGADO (bom para depuracao)

// CONFIG3H
#pragma config MCLRE = ON        // Pino de Master Clear Reset HABILITADO
#pragma config LPT1OSC = OFF     // Timer1 Low-power Oscillator DESLIGADO
#pragma config PBADEN = OFF      // Pinos do PORTB<4:0> configurados como I/O digital na inicializacao

// CONFIG4L
#pragma config STVREN = ON       // Reset por estouro de pilha (Stack) HABILITADO
#pragma config LVP = OFF         // Low-Voltage Programming DESLIGADO (essencial para usar RB5 como I/O)
#pragma config ICPRT = OFF       // In-Circuit Debug/Programming Port desabilitado
#pragma config XINST = OFF       // Instrucoes estendidas (formato C18) DESLIGADO
#pragma config DEBUG = OFF       // Modo de depuracao em background DESLIGADO

#define _XTAL_FREQ 48000000UL    // Definicao da frequencia do clock para as funcoes de delay.
                                 // ESTE VALOR PRECISA SER IGUAL AO CLOCK DA CPU DEFINIDO NOS BITS DE CONFIGURACAO

// Prototipos delays XLCD
void DelayFor18TCY(void);
void DelayPORXLCD(void);
void DelayXLCD(void);

// Delays usados pelo driver XLCD
void DelayFor18TCY(void) { __delay_us(10); }
void DelayPORXLCD(void) { __delay_ms(8); }
void DelayXLCD(void) { __delay_ms(3); }

// Variaveis globais do relogio
volatile unsigned char horas = 0;
volatile unsigned char minutos = 0;
volatile unsigned char segundos = 0;

// O contador de eventos de CCP1 é configurado para contar eventos de captura a cada ~43,69 ms. 
// Com clock de 48 MHz, o Timer1 opera com Fosc/4 = 12 MHz e prescaler 1:8, resultando em 1.5 MHz (666.67 ns por incremento).
// Para alcançar 1 segundo, são necessários 23 eventos de CCP1
volatile unsigned char ccp_events = 0;
const unsigned char EVENTS_PER_SEC = 23; // 23

char lcd_buf[9];

void interrupt isr(void)
{
    // Verifica CCP1 match
    if (PIR1bits.CCP1IF)
    {
        PIR1bits.CCP1IF = 0; // limpar flag
        ccp_events++;
        if (ccp_events >= EVENTS_PER_SEC)
        {
            ccp_events = 0;
            // incrementar relógio
            segundos++;
            LATBbits.LATB0 ^= 1;
            if (segundos >= 60) {
                segundos = 0;
                minutos++;
                if (minutos >= 60) {
                    minutos = 0;
                    horas++;
                    if (horas >= 24) horas = 0;
                }
            }
        }
    }
}

void put_TimeLCD(void)
{
    // Formata HH:MM:SS
    sprintf(lcd_buf, "%02u:%02u:%02u", horas, minutos, segundos);
    while (BusyXLCD());
    SetDDRamAddr(0x07); // primeira linha, pos 8
    putsXLCD(lcd_buf);
}

void main(void) {
    ADCON1 = 0x0F; // Configura todas as portas como digitais
    
    // Configura Timer1:
    // TMR1CS = 0 -> clock interno (Fosc/4)
    // T1CKPS = 11 -> prescaler 1:8
    T1CON = 0;
    T1CONbits.T1OSCEN = 0;
    T1CONbits.TMR1CS = 0;
    T1CONbits.T1CKPS1 = 1;
    T1CONbits.T1CKPS0 = 1;
    TMR1H = 0;
    TMR1L = 0;
    // Liga Timer1
    T1CONbits.TMR1ON = 1;
    
    // Configura CCP1 em modo Compare 
    CCP1CON = 0b00001000;
    CCPR1H = 0xFF;
    CCPR1L = 0xFF;
    
    // Configura portas do LCD
    TRISD = 0x00; 
    TRISEbits.TRISE0 = 0;
    TRISEbits.TRISE1 = 0;
    TRISEbits.TRISE2 = 0;
    OpenXLCD(EIGHT_BIT & LINES_5X7);
    while (BusyXLCD());
    WriteCmdXLCD(0x01);
    DelayPORXLCD();
    while (BusyXLCD());
    SetDDRamAddr(0x00); // primeira linha, pos 1
    putrsXLCD("Tempo: ");
    
    //Saída segundos
    TRISBbits.TRISB0 = 0;
    
    // Limpa flag e habilita interrupcoes
    PIR1bits.CCP1IF = 0;
    PIE1bits.CCP1IE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;


    while (1) {
        put_TimeLCD();
        for(int i = 0; i < 20; i++){
            __delay_ms(10);
        } 
    }
}