/*
 * File:   main.c
 * Author: Bruno Carboni & Rodrigo Pereira
 *
 * Created on 2 de Julho de 2025, 09:08
 * 
 * LAB 1: Para demonstrar o funcionamento das interrupçoes vamos
 * desenvolver um programa que acione os LEDs de forma diferente no programa 
 * principal e nas interrupcoes de baixa e alta prioridade. A interrupcao de 
 * baixa prioridade sera a interrupçao INT2 e a interrupcao de alta prioridade 
 * sera INT1.
 * 
 * * No programa principal os leds ficam todos piscando ao mesmo tempo, em uma 
 * frequenciade 2Hz.
 * 
 * * Quando ocorrer uma interrupcao de baixa prioridade sera feito um 
 * deslocmento de LED acionado da esquerda para a direita e da direita para 
 * esquerda, 3 vezes. Cada LED permanece acesso por 200ms.
 * 
 * * Quando ocorrer a interrupcao de alta prioridade, os LEDs serao acionados 
 * intercaladamente (10101010), ter?o seus estados invertidos a cada 500ms, por 
 * 3 vezes.
 * 
 * Nota: Esse exemplo eh para fins didaticos somente. Eh uma boa pratica que 
 * (1) nao se use delays dentro de uma ISR (Interrupt Service Routine) e
 * que (2) as rotina de interrupcao nao durem um longo tempo.
 * 
 */

#include <xc.h>

// CONFIG1L
#pragma config PLLDIV = 5        // Divide clock cristal por 5(20 MHz/5=4 MHz)
#pragma config CPUDIV = OSC1_PLL2// Clock CPU vem PLL(96MHz)dividido por 2=48MHz
#pragma config USBDIV = 2        // Clock USB vem PLL(96MHz)dividido por 2=48MHz

// CONFIG1H
#pragma config FOSC = HSPLL_HS   // Usa oscilador HS(High Speed)com PLL 
// habilitado

// CONFIG2L
#pragma config PWRT = ON         // Power-up Timer habilitado(estabiliza 
// alimenta??o na partida)
#pragma config BOR = ON          // Brown-out Reset habilitado (reseta o PIC se 
// a tensao cair)
#pragma config BORV = 3          // N?vel de Brown-out Reset em 2.1V
#pragma config VREGEN = ON       // Regulador de tens?o interno do USB 
// habilitado (essencial se usar USB)

// CONFIG2H
#pragma config WDT = OFF         // Watchdog Timer DESLIGADO (bom para depura??o)

// CONFIG3H
#pragma config MCLRE = ON        // Pino de Master Clear Reset HABILITADO
#pragma config LPT1OSC = OFF     // Timer1 Low-power Oscillator DESLIGADO
#pragma config PBADEN = OFF      // Pinos do PORTB<4:0> configurados como I/O 
// digital na inicializa??o

// CONFIG4L
#pragma config STVREN = ON       // Reset por estouro de pilha (Stack) 
// HABILITADO
#pragma config LVP = OFF         // Low-Voltage Programming DESLIGADO (essencial
// para usar RB5 como I/O)
#pragma config ICPRT = OFF       // In-Circuit Debug/Programming Port 
// desabilitado
#pragma config XINST = OFF       // Instrucoes estendidas(formato C18) DESLIGADO
#pragma config DEBUG = OFF       // Modo de depuracaoo em background DESLIGADO


#define LEDS LATD
#define LED0 LATDbits.LATD0      
#define LED1 LATDbits.LATD1
#define LED2 LATDbits.LATD2
#define LED3 LATDbits.LATD3
#define LED4 LATDbits.LATD4
#define LED5 LATDbits.LATD5
#define LED6 LATDbits.LATD6
#define LED7 LATDbits.LATD7

#define _XTAL_FREQ 48000000UL    // Definicao da frequ?ncia do clock para as 
// funcoes de delay.
// ESTE VALOR PRECISA SER IGUAL AO CLOCK DA CPU 
// DEFINIDO NOS BITS DE CONFIGURA??O!


void delay_200ms();
void delay_500ms();
void delay_1s();
volatile int tempo_1s = 0;
volatile int tempo_200ms = 0;

void main(void) {
    ADCON1 = 0x0F; // Garante que os pinos sejam digitais
    CMCON = 0x07; // Desliga os comparadores

    TRISBbits.TRISB1 = 1; // Configura o pino RB1/INT1 como input
    TRISBbits.TRISB2 = 1; // Configura o pino RB2/INT2 como input 
    TRISD = 0x00; // Configura todos pinos do PORTD como output

    //Para exercicio 1.1 e 1.3:
    RCONbits.IPEN = 1; // Habilita niveis de prioridade em interrupcoes
    //Para exercicio 1.2: 
    //RCONbits.IPEN = 0; 
    INTCON3bits.INT1IF = 0; // Zera a flag da interrupcao externa 1
    INTCON3bits.INT2IF = 0; // Zera a flag da interrupcao externa 2
    INTCON3bits.INT1IE = 1; // Habilita a interrupcao externa INT1
    INTCON3bits.INT2IE = 1; // Habilida a interrupcao externa INT2
    INTCON2bits.INTEDG1 = 0; // Configura interrupcao INT1 p/ borda decida
    INTCON2bits.INTEDG2 = 0; // Configura interrupcao INT2 p/ borda decida
    INTCON3bits.INT1IP = 1; // Configura interrupcao INT1 alta prioridade
    INTCON3bits.INT2IP = 0; // Configura interrupcao INT2 baixa prioridade
    INTCONbits.GIEH = 1; // Habilita todas interrupcos alta prioridade
    INTCONbits.GIEL = 1; // Habilita todas interrupcoes baixa prioridade
    INTCON2bits.RBPU = 0; // Habilita Pull-Up para o PORTB

    LEDS = 0b11111111; // Inicializa com os LEDS em nivel baixo

    //Exercicio 1.3:
    
    while(1){
        //RD0 = LED7 Como referencia de tempo.
        LATDbits.LATD7 = 1;
        delay_500ms();
        LATDbits.LATD7 = ~LATDbits.LATD7;
        delay_500ms();
        
        if(tempo_1s == 1){
            //RD1 = LED0
            LATDbits.LATD0 = 1;
            delay_200ms();
            LATDbits.LATD0 = ~LATDbits.LATD0;
            delay_200ms();
            
            LATDbits.LATD5 = 1;
        }
        
        if(tempo_200ms == 1){
            //RD1 = LED0
            LATDbits.LATD0 = 1;
            delay_1s();
            LATDbits.LATD0 = ~LATDbits.LATD0;
            delay_1s();
            
            LATDbits.LATD2 = 1;
        }
    }
    
    
    //Exercicio 1.1 e 1.2:
    /*
    while (1) {
        LEDS = 0x00;
        delay_500ms();
        LEDS = 0xFF;
        delay_500ms();
    }
    */

    return;
}


void delay_200ms() {
    for (int i = 0; i < 20; i++) {
        __delay_ms(10);
    }
}

void delay_500ms() {
    for (int i = 0; i < 50; i++) {
        __delay_ms(10);
    }
}

void delay_1s() {
    for (int i = 0; i < 100; i++) {
        __delay_ms(10);
    }
}