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

#define _XTAL_FREQ 48000000UL

#define LEDS LATD
#define LED0 LATDbits.LATD0      
#define LED1 LATDbits.LATD1
#define LED2 LATDbits.LATD2
#define LED3 LATDbits.LATD3
#define LED4 LATDbits.LATD4
#define LED5 LATDbits.LATD5
#define LED6 LATDbits.LATD6
#define LED7 LATDbits.LATD7



#define DISPLAY1_ENABLE LATEbits.LATE2 // DEFINIR LE2
#define DISPLAY2_ENABLE LATEbits.LATE0 // DEFINIR LE0
#define DISPLAY3_ENABLE LATAbits.LATA2 // DEFINIR LA2
#define DISPLAY4_ENABLE LATAbits.LATA5 // DEFINIR LA5

#define INC_BUTTON PORTBbits.RB0      // DEFINIR OS BOTOES RB0
#define DEC_BUTTON PORTBbits.RB1      // DEFINIR OS BOTOES RB1

#define DISPLAY_VALUE LATD

const char display_values[] = {
    0b00111111,	// 0 
    0b00000110,	// 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111, // 9
    0b01110111, // A
    0b01111100, // B 
    0b00111001, // C
    0b01011110, // D
    0b01111001, // E
    0b01110001  // F
};

void delay_100ms();

void main(void) {
    ADCON1 = 0x0F; // Garante que os pinos sejam digitais
    CMCON = 0x07; // Desliga os comparadores
    
    // Habilitar Pull-Up para o PORTB
    INTCON2bits.RBPU = 0;
    
    // Configurar todos os pinos do PORTD como output
    TRISD = 0x00;
    // Configurar RA2 como saída
    TRISAbits.TRISA2 = 0;
    // Configurar RA5 como saída
    TRISAbits.TRISA5 = 0;
    // Configurar RE0 como saída
    TRISEbits.TRISE0 = 0;
    // Configurar RE2 como saída
    TRISEbits.TRISE2 = 0;
            
    // Configurar RB0 como entrada 
    TRISBbits.TRISB0 = 1;
    // Configurar RB1 como entrada 
    TRISBbits.TRISB1 = 1;
    
    
    DISPLAY1_ENABLE = 1;
    DISPLAY2_ENABLE = 0;
    DISPLAY3_ENABLE = 0;
    DISPLAY4_ENABLE = 0;
    
    int i = 0;
    DISPLAY_VALUE = display_values[0];
    
    while (1){
        if(!INC_BUTTON && i < 15){
            DISPLAY_VALUE = display_values[++i];
            while(INC_BUTTON == 0);
            delay_100ms();
        } 

        if(!DEC_BUTTON && i > 0){
            DISPLAY_VALUE = display_values[--i];
            while(DEC_BUTTON == 0);
            delay_100ms();
        }   
    }
}

void delay_100ms() {
    // 10 vezes * 10 ms = 100 ms
    for (int i = 0; i < 10; i++) {
        __delay_ms(10);
    }
}