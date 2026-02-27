#include <xc.h>

// CONFIGURAÇÕES

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

#define _XTAL_FREQ 48000000UL

// DISPLAYS
#define DISPLAY1_ENABLE LATEbits.LATE2
#define DISPLAY2_ENABLE LATEbits.LATE0
#define DISPLAY3_ENABLE LATAbits.LATA2
#define DISPLAY4_ENABLE LATAbits.LATA5
#define DISPLAY_VALUE LATD

// BOTOES
#define INC_BUTTON PORTBbits.RB1
#define DEC_BUTTON PORTBbits.RB2

// BUZZER
#define BUZZER_PIN LATCbits.LATC2

const char display_values[] = {
    0b00111111,0b00000110,0b01011011,0b01001111,
    0b01100110,0b01101101,0b01111101,0b00000111,
    0b01111111,0b01101111
};

void mostra_valor(int valor);
void debounce_delay(void);
void delay_us_custom(unsigned int us);


void main(void){
    ADCON1 = 0x0F; // PORTA/B digital
    CMCON = 0x07;  // Comparadores desligados
    INTCON2bits.RBPU = 0; // Tudo digital

    // Configura pinos
    TRISBbits.TRISB1 = 1; // DEC_BUTTON
    TRISBbits.TRISB2 = 1; // INC_BUTTON
    TRISCbits.TRISC2 = 0; // BUZZER
    // Display
    TRISD = 0x00;
    // Display enable
    TRISAbits.TRISA2 = 0;
    TRISAbits.TRISA5 = 0;
    TRISEbits.TRISE0 = 0;
    TRISEbits.TRISE2 = 0;

    BUZZER_PIN = 0;

    int freq_hz = 440; // Frequência inicial
    unsigned int half_period_us;

    while(1){
        // ================= Botões inc/dec =================
        if(!INC_BUTTON && freq_hz < 9999){
            freq_hz += 100;
            while(!INC_BUTTON); debounce_delay();
        }
        if(!DEC_BUTTON && freq_hz > 0){
            freq_hz -= 100;
            while(!DEC_BUTTON); debounce_delay();
        }

        // ================= Calcula meio-período =================
        if(freq_hz > 0)
            half_period_us = 1000000UL / (2 * freq_hz);
        else
            half_period_us = 1000; // default

        // ================= Toca buzzer =================
        BUZZER_PIN = 1; 
        delay_us_custom(half_period_us);
        BUZZER_PIN = 0;
        delay_us_custom(half_period_us);

        // ================= Atualiza display =================
        mostra_valor(freq_hz);
    }
}

// ================= FUNÇÕES =================
void mostra_valor(int valor){
    int milhar = valor / 1000;
    int centena = (valor / 100) % 10;
    int dezena = (valor / 10) % 10;
    int unidade = valor % 10;

    DISPLAY1_ENABLE=1; DISPLAY2_ENABLE=0; DISPLAY3_ENABLE=0; DISPLAY4_ENABLE=0;
    DISPLAY_VALUE = display_values[milhar]; __delay_ms(2);
    DISPLAY1_ENABLE=0; DISPLAY2_ENABLE=1; DISPLAY3_ENABLE=0; DISPLAY4_ENABLE=0;
    DISPLAY_VALUE = display_values[centena]; __delay_ms(2);
    DISPLAY1_ENABLE=0; DISPLAY2_ENABLE=0; DISPLAY3_ENABLE=1; DISPLAY4_ENABLE=0;
    DISPLAY_VALUE = display_values[dezena]; __delay_ms(2);
    DISPLAY1_ENABLE=0; DISPLAY2_ENABLE=0; DISPLAY3_ENABLE=0; DISPLAY4_ENABLE=1;
    DISPLAY_VALUE = display_values[unidade]; __delay_ms(2);
}

void debounce_delay(void) { 
    for(int i=0;i<5;i++) __delay_ms(10); 
}

// Delay customizado em µs (para otimizar aproximacao da freq)
// Melhor seria usar interrupções
void delay_us_custom(unsigned int us){
    if(us < 5) {
        __delay_us(us);
    } else {
        unsigned int loops = us / 5;
        for(unsigned int i = 0; i < loops; i++)
            __delay_us(5);
        __delay_us(us % 5);
    }
}
