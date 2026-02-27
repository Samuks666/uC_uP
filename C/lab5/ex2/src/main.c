#include <xc.h>

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
                                 
// -------------------------------
// VARIÁVEIS DE DUTY CYCLE
// -------------------------------
unsigned int duty = 0;     // 0–100

// -------------------------------
// FUNÇÃO PARA ATUALIZAR O PWM
// -------------------------------
void setPWM(unsigned int d) {
    d = (d > 100) ? 100 : d;

    unsigned int pwm = (d * 1500UL) / 100;

    CCPR2L = pwm >> 2;
    CCP2CONbits.DC2B = pwm & 3;
}

void main(void) {

    // -------------------------------
    // CONFIGURAÇÃO DOS BOTÕES
    // -------------------------------
    TRISBbits.TRISB0 = 1; // Botão +
    TRISBbits.TRISB1 = 1; // Botão -
    
    // -------------------------------
    // CONFIGURAÇÃO DO PWM CCP2
    // -------------------------------
    TRISCbits.TRISC1 = 0; // RC1 = CCP2 saída

    PR2 = 374;                  // Frequência = 2 kHz
    T2CONbits.T2CKPS = 0b11;    // Prescaler = 16
    T2CONbits.TMR2ON = 1;       // Liga Timer2

    CCP2CON = 0b00001100;       // Modo PWM

    __delay_ms(10);
    setPWM(10);                  // Inicia com 10%

    // -------------------------------
    // LOOP PRINCIPAL
    // -------------------------------
    while(1) {

        // Incremento (RB0)
        if(PORTBbits.RB0 == 0){
            if(duty < 100) duty += 10;
            setPWM(duty);
            for(int i = 0; i < 30;i++){
            	__delay_ms(10);
            }
        }

        // Decremento (RB1)
        if(PORTBbits.RB1 == 0){
            if(duty > 0) duty -= 10;
            setPWM(duty);
			for(int i = 0; i < 30;i++){
				__delay_ms(10);
			}
        }
    }
}
