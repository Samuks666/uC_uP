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

void debounce_delay();
/*Frequência desejada: 4 kHz

1) Cálculo do período total do sinal:
       f = 4 kHz = 4000 Hz
       T = 1 / f = 1 / 4000 = 0,00025 s = 250 µs

2) O sinal é quadrado ? 50% do tempo em nível alto e 50% em nível baixo:
       Tempo em nível alto  = T / 2 = 125 µs
       Tempo em nível baixo = T / 2 = 125 µs
*/
void delay_125us();

void main(void) {
    ADCON1 = 0x0F; // PORTA e PORTB como digital
    CMCON = 0x07;  // Desliga comparadores
    
    INTCON2bits.RBPU = 0; // Ativa pull-ups
    
    TRISBbits.TRISB1 = 1; // RB1 como entrada (INT2)
    TRISBbits.TRISB2 = 1; // RB2 como entrada (INT1)
    TRISCbits.TRISC2 = 0; // RC2 como saída (buzzer)
    
    BUZZER_PIN = 0; // Começa desligado

    while (1) {
        if(!INT1) { // Pressionou INT1
            debounce_delay();
            while(!INT1); // Espera soltar o botão

            while(1) {
                // Gera sinal de 4kHz
                BUZZER_PIN = 1;
                delay_125us();
                BUZZER_PIN = 0;
                delay_125us();

                // Verifica se INT2 foi pressionado
                if(!INT2) {
                    debounce_delay();
                    while(!INT2); // Espera soltar o botão
                    BUZZER_PIN = 0; // Desliga buzzer
                    break; // Sai do loop
                }
            }
        }
    }
}

void debounce_delay() {
    // 5 vezes * 10 ms = 50ms
    for (int i = 0; i < 5; i++) {
        __delay_ms(10);
    }
}

void delay_125us() {
    for (int i = 0; i < 25; i++) {
        __delay_us(5);
    }
}