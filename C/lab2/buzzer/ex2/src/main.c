#include <xc.h>

// CONFIG1L
#pragma config PLLDIV = 5        // Divide o clock do cristal por 5 (20 MHz / 5 = 4 MHz)
#pragma config CPUDIV = OSC1_PLL2// Clock do CPU vem do PLL (96MHz) dividido por 2 = 48MHz
#pragma config USBDIV = 2        // Clock do USB vem do PLL (96MHz) dividido por 2 = 48MHz

// CONFIG1H
#pragma config FOSC = HSPLL_HS   // Usa o oscilador HS (High Speed) com o PLL habilitado

// CONFIG2L
#pragma config PWRT = ON         // Power-up Timer habilitado (estabiliza a alimentação na partida)
#pragma config BOR = ON          // Brown-out Reset habilitado (reseta o PIC se a tensão cair)
#pragma config BORV = 3          // Nível de Brown-out Reset em 2.1V
#pragma config VREGEN = ON       // Regulador de tensão interno do USB habilitado (essencial se usar USB)

// CONFIG2H
#pragma config WDT = OFF         // Watchdog Timer DESLIGADO (bom para depuração)

// CONFIG3H
#pragma config MCLRE = ON        // Pino de Master Clear Reset HABILITADO
#pragma config LPT1OSC = OFF     // Timer1 Low-power Oscillator DESLIGADO
#pragma config PBADEN = OFF      // Pinos do PORTB<4:0> configurados como I/O digital na inicialização

// CONFIG4L
#pragma config STVREN = ON       // Reset por estouro de pilha (Stack) HABILITADO
#pragma config LVP = OFF         // Low-Voltage Programming DESLIGADO (essencial para usar RB5 como I/O)
#pragma config ICPRT = OFF       // In-Circuit Debug/Programming Port desabilitado
#pragma config XINST = OFF       // Instruções estendidas (formato C18) DESLIGADO
#pragma config DEBUG = OFF       // Modo de depuração em background DESLIGADO

#define _XTAL_FREQ 48000000UL    // Definição da frequência do clock para as funções de delay.
                                 // ESTE VALOR PRECISA SER IGUAL AO CLOCK DA CPU DEFINIDO NOS BITS DE CONFIGURAÇÃO

#define BUZZER_PIN LATCbits.LATC2   // DEFINIR LATC2 DO BUZZER -> VER ESQUEMÁTICO DO KIT
#define INT1 PORTBbits.RB2      // DEFINIR A PORTA DO INT1
#define INT2 PORTBbits.RB1      // DEFINIR A PORTA DO INT2

void debounce_delay(void);
void delay_119us(void);  // para 4.2 kHz
void delay_208us(void);  // para 2.4 kHz
void play_4k2Hz_1s(void);
void play_2k4Hz_1s(void);

void main(void) {
    ADCON1 = 0x0F;
    CMCON = 0x07;
    INTCON2bits.RBPU = 0;

    TRISBbits.TRISB1 = 1; // INT2
    TRISBbits.TRISB2 = 1; // INT1
    TRISCbits.TRISC2 = 0; // BUZZER
    BUZZER_PIN = 0;

    char stop_flag = 0;

    while (1) {
        if(!INT1) {
            debounce_delay();
            while(!INT1);
            stop_flag = 0;

            while(!stop_flag) {

                play_4k2Hz_1s();
                if(!INT2) stop_flag = 1;
                if(stop_flag) break;

                play_2k4Hz_1s();
                if(!INT2) stop_flag = 1;
            }

            debounce_delay();
            while(!INT2);
            BUZZER_PIN = 0;
        }
    }
}

/* -------------------------------------------------
   Toca 4.2 kHz durante 1 segundo (~119 µs por nível)
-------------------------------------------------- */
void play_4k2Hz_1s(void) {
    for (int i = 0; i < 2000; i++) {
        BUZZER_PIN = 1;
        delay_119us();
        BUZZER_PIN = 0;
        delay_119us();
    }
}

/* -------------------------------------------------
   Toca 2.4 kHz durante 1 segundo (~208 µs por nível)
-------------------------------------------------- */
void play_2k4Hz_1s(void) {
    for (int i = 0; i < 1200; i++) {
        BUZZER_PIN = 1;
        delay_208us();
        BUZZER_PIN = 0;
        delay_208us();
    }
}


void delay_119us(void) {
    for (int i = 0; i < 24; i++) {
        __delay_us(5);  // 24 × 5 = 120 µs ? 119 µs
    }
}

void delay_208us(void) {
    for (int i = 0; i < 42; i++) {
        __delay_us(5);  // 42 × 5 = 210 µs ? 208 µs
    }
}

void debounce_delay(void) {
    // 5 vezes * 10 ms = 50ms
    for (int i = 0; i < 5; i++) {
        __delay_ms(10);
    }
}